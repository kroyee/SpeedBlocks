#ifndef AIMANAGER_H
#define AIMANAGER_H

#include "AI.h"

class AIManager {
	std::deque<AI> bots;
	uint8_t alive;
	sf::Clock& gameclock;
	float playersIncomingLines;
	uint16_t playerScore;
public:
	AIManager(sf::Clock& _gameclock);
	void setAmount(unsigned int amount);
	void setSpeed(uint16_t speed);
	bool update(const sf::Time& t);
	void startCountdown();
	void countDown(int count);
	void startRound();
	void endRound(const sf::Time& t);
	void distributeLines(int id, int lines);
	void resetScore();
	void setScore(GameplayData & data);
	bool empty();
};

#endif