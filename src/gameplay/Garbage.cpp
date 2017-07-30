#include "Garbage.h"

const sf::Time initialDelay = sf::milliseconds(1000);
const sf::Time freezeDelay = sf::milliseconds(450);
const sf::Time addDelay = sf::milliseconds(450);

sf::Uint16 GarbageHandler::count() {
	sf::Uint16 total=0;
	for (auto& garb : garbage)
		total+=garb.count;
	return total;
}

void GarbageHandler::clear() {
	garbage.clear();
	linesBlocked=0;
	minRemaining=initialDelay;
}

void GarbageHandler::add(sf::Uint16 amount, const sf::Time& _time) {
	garbage.push_back(Garbage(amount, _time + initialDelay));
}

sf::Uint16 GarbageHandler::block(sf::Uint16 amount, const sf::Time& _time, bool freeze_incoming) {
	if (garbage.empty())
		return amount;
	sf::Time delay = garbage.front().delay;
	
	while (amount && !garbage.empty()) {
		garbage.front().count--;
		amount--;
		linesBlocked++;
		if (garbage.front().count == 0)
			garbage.pop_front();
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