#ifndef GAMEDATASENDER_H
#define GAMEDATASENDER_H

#include <SFML/Network.hpp>
class gamePlay;

class GameDataSender {
	gamePlay & game;
	sf::Time sendTime;
	uint8_t count;
public:
	GameDataSender(gamePlay&);

	void sendstate();
	void state();
	void gameover(int winner);
	void reset();
};

#endif