#ifndef DROPDELAY_H
#define DROPDELAY_H

#include <SFML/System.hpp>

class DropDelay {
	sf::Time dropDelay, dropDelayTime;
	sf::Time increaseDropDelay, increaseDropDelayTime;
public:
	bool check(const sf::Time& t);
	void reset(const sf::Time& t);
	void clear();
};

#endif