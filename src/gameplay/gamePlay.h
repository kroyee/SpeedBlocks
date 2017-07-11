#ifndef GAMEPLAY_H
#define GAMEPLAY_H

#include <SFML/Graphics.hpp>
#include <deque>
#include "pieces.h"
#include "randomizer.h"
#include "gameField.h"
#include "Resources.h"
#include "Recording.h"
#include "BPMCount.h"

class soundBank;

class garbageClass {
public:
	garbageClass(short c, sf::Time t) { count=c; delay=t; }
	short count;
	sf::Time delay;
};

class gamePlay {
public:
	gamePlay(Resources& _resources);

	gameField field;
	basePieces basepiece[7];
	optionSet& options;

	Resources& resources;

	randomizer rander;

	Recording recorder;

	BPMCount bpmCounter;

	sf::Clock gameclock;

	sf::Time dropDelay, dropDelayTime;
	sf::Time increaseDropDelay, increaseDropDelayTime;

	sf::Time rKeyTime;
	sf::Time lKeyTime;
	sf::Time dKeyTime;

	sf::Time comboStart;
	sf::Time comboTime;

	sf::Time countDownTime;
	short countDowncount;

	sf::Time lockDownTime;
	bool lockdown;

	sf::Uint16 linesSent;
	sf::Uint16 linesRecieved;
	short comboCount;
	sf::Uint8 maxCombo;
	sf::Uint16 linesPerMinute;
	sf::Uint16 linesBlocked;
	sf::Uint16 bpm;
	sf::Uint16 garbageCleared;
	sf::Uint16 linesCleared;
	
	short pieceCount;

	std::deque<garbageClass> garbage;

	sf::Uint8 nextpiece;

	sf::Text pressEnterText;
	bool showPressEnterText;

	bool gameover;
	bool sendgameover;
	bool winner;
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
	void copyPiece(sf::Uint8 np);

	void draw();

	void delayCheck();

	void setPieceOrientation();
	void updateBasePieces();

	void sendLines(sf::Vector2i lines);
	void playComboSound(sf::Uint8 combo);

	void addGarbage(short add);
	void pushGarbage();
	void addGarbageLine(sf::Uint8 hole);
	void clearGarbage();

	bool setComboTimer();

	void startCountdown();
	bool countDown();
	void countDown(short);

	bool gameOver();

	void drawNextPiece();

	void addRecEvent(sf::Uint8 type, sf::Uint8 value);
	void startReplay();
	bool playReplay();
	void updateReplayText(RecordingEvent& event);
};

#endif