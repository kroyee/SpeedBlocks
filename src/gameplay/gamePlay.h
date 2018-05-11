#ifndef GAMEPLAY_H
#define GAMEPLAY_H

#include <SFML/Graphics.hpp>
#include <deque>
#include "pieces.h"
#include "randomizer.h"
#include "gameField.h"
#include "Recording.h"
#include "BPMCount.h"
#include "Garbage.h"
#include "Combo.h"
#include "GameSignals.h"
#include "Options.h"
#include "GameDataSender.h"
#include "DropDelay.h"
#include "AIManager.h"
#include "GameplayGameState.h"

class soundBank;
class GPBaseState;

class garbageClass {
public:
	garbageClass(short c, sf::Time t) { count=c; delay=t; }
	short count;
	sf::Time delay;
};

class gamePlay {
public:
	gamePlay(Resources& _resources);
	~gamePlay();

	gameField field;
	basePieces basepiece[7];

	Resources& resources;

	randomizer rander;

	Recording recorder;

	GameplayData data;

	BPMCount bpmCounter;
	GarbageHandler garbage;
	ComboCounter combo;
	GameDataSender dataSender;
	DropDelay pieceDropDelay;

	AIManager aiManager;

	std::unique_ptr<GPBaseState> state;

	sf::Clock gameclock;

	sf::Time rKeyTime;
	sf::Time lKeyTime;
	sf::Time dKeyTime;

	sf::Time countDownTime;
	short countDowncount;

	sf::Time lockDownTime;
	bool lockdown;

	uint8_t nextpiece, nextpieceCopy;

	sf::Text pressEnterText;
	bool showPressEnterText;

	bool autoaway;
	bool drawMe;

	bool rKey;
	bool lKey;
	bool dKey;

	void startGame();

	void mRKey();
	void mLKey();
	void mDKey();

	void sRKey() { rKey=false; }
	void sLKey() { lKey=false; }
	void sDKey() { dKey=false; }

	void hd();
	void rcw();
	void rccw();
	void r180();

	void addPiece(const sf::Time&);
	void makeNewPiece();
	void copyPiece(uint8_t np);

	void draw();

	void makeDrawCopy();

	void delayCheck();

	void setPieceOrientation();
	void updateBasePieces();

	void sendLines(sf::Vector2i lines);
	void playComboSound(uint8_t combo);

	void addGarbage(int amount);
	void pushGarbage();
	void addGarbageLine();
	void addGarbageLine(uint8_t hole);
	void clearGarbage();

	bool setComboTimer();

	void startCountdown();
	bool countDown();
	bool countDown(short);
	void startSetup(int type);

	void gameOver(int winner);

	void ready();
	void away();
	void setAway(bool away);

	void drawNextPiece();

	void addRecEvent(uint8_t type, uint8_t value);
	void startReplay();
	bool playReplay();
	void updateReplayText(RecordingEvent& event);

	void setBackgroundColor(int val);
	void setDrawMe();
	void setName(const std::string& name);
	void updateReplayScreen();
};

#endif
