#ifndef GAMEPLAY_H
#define GAMEPLAY_H

#include <SFML/Graphics.hpp>
#include <deque>
#include "pieces.h"
#include "optionSet.h"
#include "randomizer.h"
#include "gameField.h"
#include "textures.h"
#include "Recording.h"

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
	basePieces piece;
	basePieces basepiece[7];
	optionSet& options;

	Resources& resources;

	randomizer rander;

	Recording recorder;

	sf::Clock dclock;
	sf::Clock iclock;
	sf::Clock keyclock;

	sf::Time ddelay;
	sf::Time idelay;

	sf::Time rKeyTime;
	sf::Time lKeyTime;
	sf::Time dKeyTime;

	sf::Time comboStart;
	sf::Time comboTime;

	sf::Time garbageDelay;

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
	
	sf::Uint16 oldbpm[10];
	sf::Uint8 oldbpmCount;
	sf::Time bpmMeasureTiming;
	short pieceCount;

	std::deque<garbageClass> garbage;

	std::deque<sf::Time> bpmCount;

	sf::Uint8 nextpiece;
	sf::Uint8 position;

	bool gameover;
	bool sendgameover;
	bool winner;
	bool autoaway;
	bool drawMe, preDrawMe;

	bool rKey;
	bool lKey;
	bool dKey;

	sf::ConvexShape comboTimer;

	sf::Text comboText;
	sf::Text pendingText;
	sf::Text countdownText;
	sf::Text bpmText;
	sf::Text awayText;
	sf::Text positionText;
	sf::Text pressEnterText;

	sf::Uint8 comboTextVal, pendingTextVal;
	sf::Uint16 bpmTextVal;

	void startGame();

	void mRKey();
	void mLKey();
	void mDKey();

	void sRKey() { rKey=false; }
	void sLKey() { lKey=false; }
	void sDKey() { dKey=false; }

	bool possible();

	bool mRight();
	bool mLeft();
	bool mDown();
	void hd();
	void rcw();
	void rccw();
	void r180();
	bool kickTest();

	void addPiece();
	void makeNewPiece();
	void copyPiece(sf::Uint8 np);

	void draw();
	void preDraw();

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
	void setComboTimerCount(sf::Uint8 count);

	void startCountdown();
	bool countDown();
	void countDown(short);

	bool gameOver();
	void drawGameOver();

	void drawText();
	void drawNextPiece();
	void drawGameText();

	void addRecEvent(sf::Uint8 type, sf::Uint8 value);
	void startReplay();
	bool playReplay();
	void updateReplayText(RecordingEvent& event);
};

#endif