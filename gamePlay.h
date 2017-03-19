#ifndef GAMEPLAY_H
#define GAMEPLAY_H

//fixes deprecation problems
#pragma warning(disable: 4996)

#include <SFML/Graphics.hpp>
#include <deque>
#include "pieces.h"
#include "optionSet.h"
#include "randomizer.h"
#include "gameField.h"
#include "textures.h"

class soundBank;

class garbageClass {
public:
	garbageClass(short c, sf::Time t) { count=c; delay=t; }
	short count;
	sf::Time delay;
};

class gamePlay {
public:
	gamePlay(textures* texy, soundBank* soundy, sf::Font* fonty) : field(texy->tile, &texy->fieldBackground){
		sounds=soundy;

		font=fonty;

		nextpiece=0;

		ddelay=sf::seconds(1);
		idelay=sf::seconds(3);

		comboTime=sf::seconds(0);

		comboTimer.setPosition(315, 240);
		comboTimer.setFillColor(sf::Color(255,0,0));

		comboCount=0;
		linesSent=0;
		maxCombo=0;
		linesRecieved=0;
		garbageCleared=0;

		gameover=false;
		sendgameover=false;
		winner=false;

		rKey=false;
		lKey=false;
		dKey=false;

		comboText.setFont(*font);
		comboText.setCharacterSize(48);
		comboText.setColor(sf::Color::White);
		comboText.setPosition(360,270);
		pendingText.setFont(*font);
		pendingText.setCharacterSize(48);
		pendingText.setColor(sf::Color::White);
		pendingText.setPosition(360,500);
		countdownText.setFont(*font);
		countdownText.setCharacterSize(96);
		countdownText.setColor(sf::Color::White);
		countdownText.setPosition(110,210);
		bpmText.setFont(*font);
		bpmText.setCharacterSize(48);
		bpmText.setColor(sf::Color::White);
		bpmText.setPosition(360, 400);
		awayText.setFont(*font);
		awayText.setCharacterSize(52);
		awayText.setColor(sf::Color::White);
		awayText.setPosition(110,110);

		updateBasePieces();
	}

	gameField field;
	basePieces piece;
	basePieces basepiece[7];
	optionSet options;

	soundBank* sounds;

	randomizer rander;

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

	bool gameover;
	bool sendgameover;
	bool winner;
	bool autoaway;

	bool rKey;
	bool lKey;
	bool dKey;

	sf::ConvexShape comboTimer;

	sf::Font* font;
	sf::Text comboText;
	sf::Text pendingText;
	sf::Text countdownText;
	sf::Text bpmText;
	sf::Text awayText;

	void startGame();

	void mRKey();
	void mLKey();
	void mDKey();

	void sRKey() { rKey=false; }
	void sLKey() { lKey=false; }
	void sDKey() { dKey=false; }

	bool possible();

	void mRight();
	void mLeft();
	void mDown();
	void hd();
	void rcw();
	void rccw();
	void r180();

	void addPiece();
	void makeNewPiece();

	void draw();
	void preDraw();

	void delayCheck();

	void setPieceOrientation();
	void updateBasePieces();

	void sendLines(sf::Vector2i lines);

	void addGarbage(short add);
	void pushGarbage();
	void clearGarbage();

	bool setComboTimer();

	void startCountdown();
	bool countDown();
	void countDown(short);

	bool gameOver();
	void drawGameOver();
};

#endif
