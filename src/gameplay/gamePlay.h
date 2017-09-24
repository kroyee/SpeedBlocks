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
#include "Garbage.h"
#include "Combo.h"

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
	GarbageHandler garbage;
	ComboCounter combo;

	sf::Clock gameclock;

	sf::Time dropDelay, dropDelayTime;
	sf::Time increaseDropDelay, increaseDropDelayTime;

	sf::Time rKeyTime;
	sf::Time lKeyTime;
	sf::Time dKeyTime;

	sf::Time countDownTime;
	short countDowncount;

	sf::Time lockDownTime;
	bool lockdown;

	sf::Uint16 linesSent;
	sf::Uint16 linesRecieved;
	sf::Uint16 linesPerMinute;
	sf::Uint16 bpm;
	sf::Uint16 garbageCleared;
	sf::Uint16 linesCleared;
	
	short pieceCount;

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

	void addGarbage(sf::Uint16 amount);
	void pushGarbage();
	void addGarbageLine(sf::Uint8 hole);
	void clearGarbage();

	bool setComboTimer();

	void startCountdown();
	bool countDown();
	bool countDown(short);

	bool gameOver();

	void ready();
	void away();
	void setAway(bool away);

	void drawNextPiece();

	void addRecEvent(sf::Uint8 type, sf::Uint8 value);
	void startReplay();
	bool playReplay();
	void updateReplayText(RecordingEvent& event);

	void setBackgroundColor(int val);
	void setDrawMe();
	void makeBackgroundLines();
	void setName(const sf::String& name);
	const sf::String& getName();
	void updateReplayScreen();

	void handleEvent(sf::Event& event);
};

#endif