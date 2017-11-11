#ifndef AI_H
#define AI_H

#include "gameField.h"
#include "randomizer.h"
#include "TestField.h"
#include "BPMCount.h"
#include "Garbage.h"
#include "Combo.h"
#include "DropDelay.h"
#include "GameSignals.h"
#include <vector>
#include <deque>
#include <thread>
#include <atomic>
#include <mutex>

class Resources;

enum class Mode { Downstack, Stack };

class AI {
public:
	Resources& resources;
	std::array<double, 10> weights, downstackWeights, stackWeights;
	obsField* field;
	TestField firstMove, secondMove;

	uint8_t nextpiece;

	sf::Vector2i well2Pos;

	GameplayData data;
	uint16_t gameCount;
	uint16_t score;

	sf::Uint16 id;
	float incomingLines;

	Mode mode;
	bool drawMe;
	std::vector<uint8_t> currentMove;
	std::deque<uint8_t> moveQueue;
	std::vector<uint8_t>::iterator moveIterator;

	sf::Time nextmoveTime, movepieceTime, moveTime, finesseTime;

	randomizer rander;
	BPMCount bpmCounter;
	GarbageHandler garbage;
	ComboCounter combo;
	DropDelay pieceDropDelay;

	sf::Clock& gameclock;

	std::thread t;		// Threading control variables
	std::atomic<uint8_t> updateField, terminateThread;
	std::mutex moveQueueMutex;
	std::atomic<bool> alive, adjustDownMove, movingPiece;

	AI(obsField& _field, sf::Clock& _gameclock);

	void setField(obsField& _field);
	void startMove();
	void continueMove();
	bool executeMove();
	void setPiece(int piece);
	void setNextPiece(int piece);

	void startAI();
	void restartGame();
	void addGarbageLine();

	void setMode(Mode, bool vary=false);
	void setSpeed(uint16_t speed);

	bool playAI();
	void aiThreadRun();

	void startRound();
	void startCountdown();
	void countDown(int count);
	void endRound(const sf::Time& t, bool winner);

	void delayCheck(const sf::Time& t);
	bool setComboTimer(const sf::Time& t);
	void sendLines(sf::Vector2i lines, const sf::Time& t);
	void addGarbage(uint16_t amount, const sf::Time& t);
};

#endif