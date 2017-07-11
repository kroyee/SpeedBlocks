#ifndef BPMCOUNT_H
#define BPMCOUNT_H

#include <SFML/Graphics.hpp>
#include <deque>

class BPMCount {
public:
	sf::Uint16 oldbpm[10];
	sf::Uint8 oldbpmCount;
	sf::Time bpmMeasureTiming;
	sf::Uint16 bpm;

	std::deque<sf::Time> bpmCount;

	void addPiece(const sf::Time&);
	sf::Uint16 calcBpm(const sf::Time&);

	void clear();
};

#endif