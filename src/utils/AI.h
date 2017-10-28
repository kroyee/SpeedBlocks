#ifndef AI_H
#define AI_H

#include "gameField.h"
#include "randomizer.h"
#include "TestField.h"
#include <deque>

class Resources;

enum class Mode { Downstack, Stack };

class AI {
public:
	Resources& resources;
	std::array<double, 8> weights, downstackWeights, stackWeights;
	obsField* field;
	TestField firstMove, secondMove;

	uint8_t nextpiece;
	int32_t score;

	sf::Vector2i well2Pos;

	uint16_t garbageCleared;
	uint16_t linesCleared;
	uint16_t moveCount;
	uint16_t gameCount;

	Mode mode;
	bool movingPiece;
	std::vector<uint8_t> currentMove;
	std::vector<uint8_t>::iterator moveIterator;

	sf::Time nextmoveTime, movepieceTime, moveTime, finesseTime;

	randomizer rander;

	AI(Resources& _res);

	void startMove(const sf::Time& t);
	bool continueMove(const sf::Time& t);
	void setPiece(int piece);
	void setNextPiece(int piece);

	void startAI(obsField & _field);
	void restartGame();
	void addGarbageLine(uint8_t);

	void setMode(Mode);
	void setSpeed(const sf::Time& t);

	bool playAI(const sf::Time& t);
};

#endif