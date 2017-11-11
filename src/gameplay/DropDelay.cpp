#include "DropDelay.h"

bool DropDelay::check(const sf::Time& t) {
	if (t - increaseDropDelayTime > increaseDropDelay) {
		if (dropDelay > sf::milliseconds(200))
			dropDelay-=sf::milliseconds(10);
		else if (dropDelay > sf::milliseconds(100))
			dropDelay-=sf::milliseconds(5);
		else if (dropDelay > sf::milliseconds(50))
			dropDelay-=sf::milliseconds(2);
		else if (dropDelay > sf::milliseconds(10))
			dropDelay-=sf::milliseconds(1);
		increaseDropDelayTime = t;
	}

	if (t - dropDelayTime > dropDelay) {
		dropDelayTime = t;
		return true;
	}
	return false;
}

void DropDelay::reset(const sf::Time& t) {
	dropDelayTime = t;
}

void DropDelay::clear() {
	increaseDropDelay=sf::seconds(3);
	increaseDropDelayTime=sf::seconds(0);
	dropDelay=sf::seconds(1);
	dropDelayTime=sf::seconds(0);
}