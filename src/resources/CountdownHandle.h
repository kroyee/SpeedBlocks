#ifndef COUNTDOWNHANDLE_H
#define COUNTDOWNHANDLE_H

#include <SFML/Network.hpp>

class CountdownHandle {
private:
	bool countdownOngoing;
	sf::Time countdownTime;
	sf::Uint8 countdownCount;
public:
	void set(const sf::Time& t, sf::Packet& packet);
	sf::Uint8 check(const sf::Time& t);
	void start(const sf::Time& t);
	void stop();
	bool ongoing();
};

#endif