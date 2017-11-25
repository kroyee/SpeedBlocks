#ifndef GARBAGE_H
#define GARBAGE_H

#include <SFML/Graphics.hpp>
#include <deque>
#include <mutex>

struct Garbage {
	Garbage(short c, const sf::Time& t) { count=c; delay=t; }
	short count;
	sf::Time delay;
};

class GarbageHandler {
private:
	std::deque<Garbage> garbage;
	std::mutex garbageMutex;
public:
	GarbageHandler(uint16_t& _linesBlocked);
	
	uint16_t & linesBlocked;
	sf::Time minRemaining, offsetTime, addDelay;

	sf::Uint16 count();
	void clear();
	void add(sf::Uint16 amount, const sf::Time& _time);
	sf::Uint16 block(sf::Uint16 amount, const sf::Time& _time, bool freeze_incoming=true);
	bool check(const sf::Time& _time);
	void setAddDelay(int delay);
	sf::Uint8 getOffset(const sf::Time& _time);
	void setOffset(const sf::Time& _time);
};

#endif