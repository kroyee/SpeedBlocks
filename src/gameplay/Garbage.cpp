#include "Garbage.h"
#include "GameSignals.h"

const sf::Time initialDelay = sf::milliseconds(1000);
const sf::Time freezeDelay = sf::milliseconds(450);

GarbageHandler::GarbageHandler(uint16_t& _linesBlocked) : linesBlocked(_linesBlocked), addDelay(sf::milliseconds(450)) {
	Signals::GameAddDelay.connect(&GarbageHandler::setAddDelay, this);
}

sf::Uint16 GarbageHandler::count() {
	sf::Uint16 total=0;
	for (auto& garb : garbage)
		total+=garb.count;
	return total;
}

void GarbageHandler::clear() {
	std::lock_guard<std::mutex> mute(garbageMutex);
	garbage.clear();
	linesBlocked=0;
	minRemaining=initialDelay;
	offsetTime=sf::seconds(0);
}

void GarbageHandler::add(sf::Uint16 amount, const sf::Time& _time) {
	std::lock_guard<std::mutex> mute(garbageMutex);
	garbage.push_back(Garbage(amount, _time + initialDelay));
}

sf::Uint16 GarbageHandler::block(sf::Uint16 amount, const sf::Time& _time, bool freeze_incoming) {
	std::lock_guard<std::mutex> mute(garbageMutex);
	if (garbage.empty())
		return amount;
	sf::Time delay = garbage.front().delay;
	
	int blocked=0;
	while (amount && !garbage.empty()) {
		garbage.front().count--;
		amount--;
		blocked++;
		if (garbage.front().count == 0)
			garbage.pop_front();
	}
	if (blocked) {
		linesBlocked+=blocked;
		Signals::SendSig(4, blocked);
	}

	if (!garbage.empty()) {
		garbage.front().delay = std::max(delay, garbage.front().delay);
		if (freeze_incoming)
			garbage.front().delay = std::min(garbage.front().delay+freezeDelay, _time+minRemaining+freezeDelay);
	}
	else minRemaining = initialDelay;

	return amount;
}

bool GarbageHandler::check(const sf::Time& _time) {
	std::lock_guard<std::mutex> mute(garbageMutex);
	if (garbage.empty())
		return false;
	if (_time > garbage.front().delay) {
		sf::Time delay = garbage.front().delay + addDelay;
		if (--garbage.front().count == 0)
			garbage.pop_front();
		if (!garbage.empty()) {
			garbage.front().delay = std::max(delay, garbage.front().delay);
			minRemaining = garbage.front().delay - _time;
		}
		else minRemaining = initialDelay;
		return true;
	}
	minRemaining = std::min(minRemaining, garbage.front().delay - _time);
	return false;
}

void GarbageHandler::setAddDelay(int delay) {
	std::lock_guard<std::mutex> mute(garbageMutex);
	addDelay = sf::milliseconds(delay);
}

sf::Uint8 GarbageHandler::getOffset(const sf::Time& _time) {
	std::lock_guard<std::mutex> mute(garbageMutex);
	if (_time > offsetTime)
		return 0;
	return ((offsetTime - _time) / addDelay) * 30;
}

void GarbageHandler::setOffset(const sf::Time& _time) {
	std::lock_guard<std::mutex> mute(garbageMutex);
	offsetTime = _time + addDelay;
}