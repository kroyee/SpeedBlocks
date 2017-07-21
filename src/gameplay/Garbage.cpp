#include "Garbage.h"

sf::Uint16 GarbageHandler::count() {
	sf::Uint16 total=0;
	for (auto& garb : garbage)
		total+=garb.count;
	return total;
}

void GarbageHandler::clear() {
	garbage.clear();
	linesBlocked=0;
}

void GarbageHandler::add(sf::Uint16 amount, const sf::Time& _time) {
	garbage.push_back(Garbage(amount, _time + sf::milliseconds(1500)));
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
			garbage.front().delay = std::min(garbage.front().delay+sf::milliseconds(500), _time+sf::milliseconds(1000));
	}

	return amount;
}

bool GarbageHandler::check(const sf::Time& _time) {
	if (garbage.empty())
		return false;
	if (_time > garbage.front().delay) {
		sf::Time delay = garbage.front().delay + sf::milliseconds(500);
		if (--garbage.front().count == 0)
			garbage.pop_front();
		if (!garbage.empty())
			garbage.front().delay = std::max(delay, garbage.front().delay);
		return true;
	}
	return false;
}