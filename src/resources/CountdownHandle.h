#ifndef COUNTDOWNHANDLE_H
#define COUNTDOWNHANDLE_H

#include <SFML/Network.hpp>

class CountdownHandle {
private:
	bool countdownOngoing;
	sf::Time countdownTime;
	uint8_t countdownCount;
public:
	void set(const sf::Time& t, sf::Packet& packet);
	uint8_t check(const sf::Time& t);
	void start(const sf::Time& t);
	void stop();
	bool ongoing();
};

#endif