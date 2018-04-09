#ifndef BPMCOUNT_H
#define BPMCOUNT_H

#include <SFML/Graphics.hpp>
#include <deque>
#include <mutex>

class BPMCount {
	std::mutex bpmMutex;
public:
	uint16_t oldbpm[10];
	uint8_t oldbpmCount;
	sf::Time bpmMeasureTiming;
	uint16_t bpm;

	std::deque<sf::Time> bpmCount;

	void addPiece(const sf::Time&);
	uint16_t calcBpm(const sf::Time&);

	void clear();
};

#endif