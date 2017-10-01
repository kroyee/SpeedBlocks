#ifndef GARBAGE_H
#define GARBAGE_H

#include <SFML/Graphics.hpp>
#include <list>

struct Garbage {
	Garbage(short c, const sf::Time& t) { count=c; delay=t; }
	short count;
	sf::Time delay;
};

class GarbageHandler {
private:
	std::list<Garbage> garbage;
public:
	GarbageHandler(uint16_t& _linesBlocked) : linesBlocked(_linesBlocked) {}
	
	uint16_t & linesBlocked;
	sf::Time minRemaining;

	sf::Uint16 count();
	void clear();
	void add(sf::Uint16 amount, const sf::Time& _time);
	sf::Uint16 block(sf::Uint16 amount, const sf::Time& _time, bool freeze_incoming=true);
	bool check(const sf::Time& _time);
};

#endif