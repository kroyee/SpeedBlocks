#include "gamePlay.h"
#include "gameField.h"
#include "optionSet.h"
#include "pieces.h"
#include "randomizer.h"
#include "sounds.h"
#include <SFML/Graphics.hpp>
#include <iostream>
#include <deque>
#include <string>
using std::to_string;
using std::cout;
using std::endl;

#define PI 3.14159265

gamePlay::gamePlay(Resources& _resources) :
field(_resources),
options(_resources.options),
resources(_resources),
showPressEnterText(true)
{
	nextpiece=0;

	dropDelay=sf::seconds(1);
	increaseDropDelay=sf::seconds(3);

	linesSent=0;
	linesRecieved=0;
	garbageCleared=0;
	linesCleared=0;

	gameover=false;
	sendgameover=false;
	winner=false;

	lockdown=false;

	rKey=false;
	lKey=false;
	dKey=false;

	field.text.setName(options.name);
	pressEnterText.setFont(resources.gfx.typewriter);
	pressEnterText.setCharacterSize(17);
	pressEnterText.setColor(sf::Color::White);
	pressEnterText.setPosition(10,500);
	pressEnterText.setString("press Enter to start practice");

	updateBasePieces();
}

void gamePlay::startGame() {
	recorder.start(field.square);
	makeNewPiece();
	drawMe=true;
	garbage.clear();
	gameclock.restart();
	combo.clear();
	dropDelay=sf::seconds(1);
	dropDelayTime=sf::seconds(0);
	linesSent=0;
	linesRecieved=0;
	garbageCleared=0;
	linesCleared=0;
	linesBlocked=0;
	pieceCount=0;
	autoaway=true;
	lockdown=false;
	bpmCounter.clear();
	if (recorder.rec)
		addRecEvent(5, 0);
}

void gamePlay::mRKey() {
	if (!rKey) {
		if (field.mRight()) {
			if (recorder.rec)
				addRecEvent(1, 0);
			drawMe=true;
		}
		rKeyTime=gameclock.getElapsedTime()+options.repeatDelay;
		lKey=false;
	}
	rKey=true;
	autoaway=false;
}

void gamePlay::mLKey() {
	if (!lKey) {
		if (field.mLeft()) {
			if (recorder.rec)
				addRecEvent(1, 0);
			drawMe=true;
		}
		lKeyTime=gameclock.getElapsedTime()+options.repeatDelay;
		rKey=false;
	}
	lKey=true;
	autoaway=false;
}

void gamePlay::mDKey() {
	if (!dKey) {
		if (field.mDown()) {
			if (recorder.rec)
				addRecEvent(1, 0);
			drawMe=true;
			dropDelayTime = gameclock.getElapsedTime();
			lockdown=false;
		}
		else {
			if (!lockdown)
				lockDownTime=gameclock.getElapsedTime()+sf::milliseconds(400);
			lockdown=true;
		}
		dKeyTime=gameclock.getElapsedTime()+options.repeatDelayDown;
	}
	dKey=true;
	autoaway=false;
}

void gamePlay::hd() {
	autoaway=false;
	drawMe=true;
	field.hd();
	addPiece(gameclock.getElapsedTime());
	sendLines(field.clearlines());
	dropDelayTime = gameclock.getElapsedTime();
	makeNewPiece();
}

void gamePlay::rcw() {
	autoaway=false;
	drawMe=true;
	if (field.rcw())
		if (recorder.rec)
			addRecEvent(1, 0);
}

void gamePlay::rccw() {
	autoaway=false;
	drawMe=true;
	if (field.rccw())
		if (recorder.rec)
			addRecEvent(1, 0);
}

void gamePlay::r180() {
	autoaway=false;
	drawMe=true;
	if (field.r180())
		if (recorder.rec)
			addRecEvent(1, 0);
}

void gamePlay::addPiece(const sf::Time& _time) {
	if (recorder.rec)
		addRecEvent(2, 0);
	field.addPiece();
	pieceCount++;
	bpmCounter.addPiece(_time);
}

void gamePlay::makeNewPiece() {
	copyPiece(nextpiece);

	nextpiece = rander.getPiece();
	lockdown=false;
	if (!field.possible()) {
		addPiece(gameclock.getElapsedTime());
		gameover=true;
		sendgameover=true;
	}

	if (recorder.rec) {
		addRecEvent(1, 0);
		addRecEvent(6, 0);
	}
}

void gamePlay::copyPiece(sf::Uint8 np) {
	for (int x=0; x<4; x++)
		for (int y=0; y<4; y++)
			field.piece.grid[y][x] = basepiece[np].grid[y][x];
	field.piece.lpiece=basepiece[np].lpiece;
	field.piece.tile=basepiece[np].tile;
	field.piece.piece=basepiece[np].piece;
	field.piece.current_rotation=basepiece[np].current_rotation;
	field.piece.posX = 3;
	field.piece.posY = 0;
}

void gamePlay::draw() {
	field.drawField();
	drawNextPiece();
	if (showPressEnterText)
		field.texture.draw(pressEnterText);
    field.texture.display();
}

void gamePlay::delayCheck() {
	if (gameclock.getElapsedTime() - dropDelayTime > dropDelay) {
		if (field.mDown()) {
			drawMe=true;
			lockdown=false;
			if (recorder.rec)
				addRecEvent(1, 0);
		}
		else {
			if (!lockdown)
				lockDownTime=gameclock.getElapsedTime()+sf::milliseconds(400);
			lockdown=true;
		}
		dropDelayTime = gameclock.getElapsedTime();
	}

	if (gameclock.getElapsedTime() - increaseDropDelayTime > increaseDropDelay) {
		if (dropDelay > sf::milliseconds(200))
			dropDelay-=sf::milliseconds(10);
		else if (dropDelay > sf::milliseconds(100))
			dropDelay-=sf::milliseconds(5);
		else if (dropDelay > sf::milliseconds(50))
			dropDelay-=sf::milliseconds(2);
		else if (dropDelay > sf::milliseconds(10))
			dropDelay-=sf::milliseconds(1);
		increaseDropDelayTime = gameclock.getElapsedTime();
	}

	sf::Time current = gameclock.getElapsedTime();
	bool update=false;
	if (rKey) {
		while (current > rKeyTime) {
			rKeyTime+=options.repeatSpeed;
			if (!field.mRight())
				break;
			else
				update=true;
		}
		if (update) {
			if (recorder.rec)
				addRecEvent(1, 0);
			drawMe=true;
		}
	}
	if (lKey) {
		update=false;
		while (current > lKeyTime) {
			lKeyTime+=options.repeatSpeed;
			if (!field.mLeft())
				break;
			else
				update=true;
		}
		if (update) {
			if (recorder.rec)
				addRecEvent(1, 0);
			drawMe=true;
		}
	}
	if (dKey) {
		update=false;
		while (current > dKeyTime) {
			dKeyTime+=options.repeatSpeedDown;
			if (!field.mDown()) {
				if (!lockdown)
					lockDownTime=gameclock.getElapsedTime()+sf::milliseconds(400);
				lockdown=true;
				break;
			}
			else {
				lockdown=false;
				update=true;
			}
		}
		if (update) {
			if (recorder.rec)
				addRecEvent(1, 0);
			drawMe=true;
			dropDelayTime = gameclock.getElapsedTime();
		}
	}

	sf::Uint16 comboLinesSent = combo.check(gameclock.getElapsedTime());
	if (comboLinesSent) {
		comboLinesSent = garbage.block(comboLinesSent, gameclock.getElapsedTime(), false);
		field.text.setPending(garbage.count());
		linesSent += comboLinesSent;
		drawMe=true;
	}

	sf::Uint16 newbpm = bpmCounter.calcBpm(gameclock.getElapsedTime());
	if (newbpm != bpm) {
		field.text.setBpm(newbpm);
		bpm = newbpm;
		drawMe=true;
	}


	if (setComboTimer())
		drawMe=true;

	if (garbage.check(gameclock.getElapsedTime())) {
		pushGarbage();
		drawMe=true;
	}

	if (lockdown && gameclock.getElapsedTime() > lockDownTime) {
		if (!field.mDown()) {
			addPiece(gameclock.getElapsedTime());
			sendLines(field.clearlines());
			drawMe=true;
			makeNewPiece();
		}
		else
			lockdown=false;
	}
}

void gamePlay::setPieceOrientation() {
	for (int x=0; x<7; x++) {
		short rotation=options.piecerotation[x];
		while (rotation > 0) {
			basepiece[x].rcw();
			rotation--;
		}
	}
}

void gamePlay::updateBasePieces() {
	for (int p=0; p<7; p++) {
		basepiece[p].posX=0;
		basepiece[p].posY=0;
		basepiece[p].lpiece=false;
		basepiece[p].tile=options.basepiece[p].tile;
		basepiece[p].rotation=options.basepiece[p].rotation;
		basepiece[p].current_rotation=options.basepiece[p].current_rotation;
		basepiece[p].piece=p;
		for (int y=0; y<4; y++)
			for (int x=0; x<4; x++)
				basepiece[p].grid[y][x] = options.basepiece[p].grid[y][x];
	}
	basepiece[4].lpiece=true;
	basepiece[6].lpiece=true;

	setPieceOrientation();
}

void gamePlay::sendLines(sf::Vector2i lines) {
	garbageCleared+=lines.y;
	linesCleared+=lines.x;
	if (lines.x==0) {
		combo.noClear();
		if (options.sound)
			resources.sounds.pieceDrop();
		return;
	}
	linesSent += garbage.block(lines.x-1, gameclock.getElapsedTime());
	field.text.setPending(garbage.count());
	combo.increase(gameclock.getElapsedTime(), lines.x);

	if (options.sound) {
		resources.sounds.lineClear();
		playComboSound(combo.comboCount);
	}

	setComboTimer();
	field.text.setCombo(combo.comboCount);
	
	if (recorder.rec)
		addRecEvent(5, 0);
}

void gamePlay::playComboSound(sf::Uint8 combo) {
	if (combo==5)
		resources.sounds.combo5();
	else if (combo==8)
		resources.sounds.combo8();
	else if (combo==10)
		resources.sounds.combo11();
	else if (combo==12)
		resources.sounds.combo13();
	else if (combo==14)
		resources.sounds.combo15();
	else if (combo==16)
		resources.sounds.combo17();
	else if (combo==18)
		resources.sounds.combo19();
	else if (combo==20)
		resources.sounds.combo21();
}

void gamePlay::addGarbage(sf::Uint16 amount) {
	garbage.add(amount, gameclock.getElapsedTime());

	linesRecieved+=amount;

	field.text.setPending(garbage.count());

	if (recorder.rec)
		addRecEvent(5, 0);

	if (options.sound)
		resources.sounds.garbAdd();
}

void gamePlay::pushGarbage() {
	field.text.setPending(garbage.count());

	sf::Uint8 hole = rander.getHole();
	addGarbageLine(hole);

	if (recorder.rec)
		addRecEvent(4, hole);

	if (!field.possible()) {
		field.piece.mup();
		if (!lockdown)
			lockDownTime=gameclock.getElapsedTime()+sf::milliseconds(400);
		lockdown=true;
		if (recorder.rec)
			addRecEvent(1, 0);
	}
}

void gamePlay::addGarbageLine(sf::Uint8 hole) {
	for (int y=0; y<21; y++)
		for (int x=0; x<10; x++)
			field.square[y][x]=field.square[y+1][x];
	for (int x=0; x<10; x++)
		field.square[21][x]=8;
	field.square[21][hole]=0;
}

bool gamePlay::setComboTimer() {
	sf::Uint8 count = combo.timerCount(gameclock.getElapsedTime());
	return field.text.setComboTimer(count);
}

void gamePlay::startCountdown() {
	countDownTime = sf::seconds(0);
	countDowncount = 3;

	gameover=false;
	field.clear();
	field.text.setCountdown(3);
	makeNewPiece();
	while (nextpiece == 2 || nextpiece == 3) {
		rander.reset();
		makeNewPiece();
	}
	field.piece.piece=7;
	combo.clear();
	lKeyTime=sf::seconds(0);
	rKeyTime=sf::seconds(0);
	garbage.clear();
	draw();
	if (recorder.rec)
		addRecEvent(7, 3);
}

bool gamePlay::countDown() {
	if (gameclock.getElapsedTime()>countDownTime) {
		countDownTime = gameclock.getElapsedTime() + sf::seconds(1);
		countDowncount--;
		field.text.setCountdown(countDowncount);
		if (recorder.rec)
			addRecEvent(7, countDowncount);
		if (countDowncount) {
			if (options.sound)
				resources.sounds.startBeep1();
			draw();
		}
		else {
			if (options.sound)
				resources.sounds.startBeep2();
			return true;
		}
	}
	return false;
}

void gamePlay::countDown(short c) {
	gameover=false;
	field.text.setCountdown(c);
	if (options.sound) {
		if (c)
			resources.sounds.startBeep1();
		else
			resources.sounds.startBeep2();
	}
	draw();
	if (recorder.rec)
		addRecEvent(7, c);
}

bool gamePlay::gameOver() {
	if (!gameover)
		return false;

	linesPerMinute = (((float)linesSent)/((float)gameclock.getElapsedTime().asSeconds()))*60.0;
	bpm = (int)(pieceCount / ((float)(gameclock.getElapsedTime().asSeconds()))*60.0);
	field.text.setBpm(bpm);

	addRecEvent(5, 0);
	recorder.stop();

    if (winner)
    	field.text.setGameover(2);
    else
		field.text.setGameover(1);

	draw();
	return true;
}

void gamePlay::drawNextPiece() {
	for (int y=0; y<4; y++)
        for (int x=0; x<4; x++)
            if (basepiece[nextpiece].grid[y][x] != 0) {
                	field.tile[basepiece[nextpiece].tile-1].setPosition(sf::Vector2f(-15*basepiece[nextpiece].lpiece+330+x*30, 45+y*30));
                	field.texture.draw(field.tile[basepiece[nextpiece].tile-1]);
            	}
}

void gamePlay::addRecEvent(sf::Uint8 type, sf::Uint8 value) {
	RecordingEvent event;
	event.type = type;
	switch (type) {
		case 1:
		case 2:
			event.piece = field.piece.piece;
			event.rotation = field.piece.current_rotation;
			event.color = field.piece.tile;
			event.pending = field.text.pending;
			event.combo = field.text.combo;
			event.bpm = field.text.bpm;
			event.comboTimer = field.text.comboTimer.getPointCount()-2;
			event.x = field.piece.posX;
			event.y = field.piece.posY;
			recorder.addEvent(event);
		break;
		case 3:
		break;
		case 4:
			event.x = value;
		case 5:
			event.pending = field.text.pending;
			event.combo = field.text.combo;
			event.bpm = field.text.bpm;
			event.comboTimer = field.text.comboTimer.getPointCount()-2;
			recorder.addEvent(event);
		break;
		case 6:
			event.piece = basepiece[nextpiece].piece;
			event.rotation = basepiece[nextpiece].current_rotation;
			event.color = basepiece[nextpiece].tile;
			event.pending = field.text.pending;
			event.combo = field.text.combo;
			event.bpm = field.text.bpm;
			event.comboTimer = field.text.comboTimer.getPointCount()-2;
			recorder.addEvent(event);
		break;
		case 7:
			event.piece = basepiece[nextpiece].piece;
			event.rotation = basepiece[nextpiece].current_rotation;
			event.color = basepiece[nextpiece].tile;
			event.pending = value;
			recorder.addEvent(event);
		break;
	}
}

void gamePlay::startReplay() {
	recorder.timer.restart();
	recorder.currentEvent = 0;
	recorder.prevCombo = 0;
	recorder.halt = false;
	recorder.startAt = sf::seconds(0);
}

bool gamePlay::playReplay() {
	if (recorder.halt)
		return true;
	sf::Time currentTime = recorder.timer.getElapsedTime() + recorder.startAt;
	for ( ; currentTime>recorder.events[recorder.currentEvent].time; recorder.currentEvent++) {
		auto&& event = recorder.events[recorder.currentEvent];

		bpmCounter.calcBpm(event.time);

		switch (event.type) {
			case 100:
				gameclock.restart();
				pieceCount=0;
				linesCleared=0;
				garbageCleared=0;
				field.clear();
				for (int y=0; y<22; y++)
					for (int x=0; x<10; x++)
						field.square[y][x] = recorder.starting_position[y][x];
				pieceCount=0;
				bpmCounter.clear();
				recorder.comboSet=sf::seconds(0);
				recorder.lastComboTimer=10;
				recorder.comboTimer=0;
			break;
			case 101:
				recorder.halt=true;
				drawMe=true;
				field.text.setBpm((int)(pieceCount / ((float)(event.time.asSeconds()))*60.0));
				return false;
			break;
			case 1:
				copyPiece(event.piece);
				while (field.piece.current_rotation != event.rotation)
					field.piece.rcw();
				field.piece.tile = event.color;
				field.piece.posX = event.x;
				field.piece.posY = event.y;
				drawMe = true;
			break;
			case 2:
			{
				copyPiece(event.piece);
				while (field.piece.current_rotation != event.rotation)
					field.piece.rcw();
				field.piece.tile = event.color;
				field.piece.posX = event.x;
				field.piece.posY = event.y;
				addPiece(event.time);
				sf::Vector2i lines = field.clearlines();
				linesCleared+=lines.x;
				garbageCleared+=lines.y;
				if (options.sound) {
					if (lines.x == 0)
						resources.sounds.pieceDrop();
					else
						resources.sounds.lineClear();
				}
				drawMe = true;
			}
			break;
			case 4:
				addGarbageLine(event.x);
				drawMe=true;
			break;
			case 5:
				updateReplayText(event);
				drawMe=true;
			break;
			case 6:
				nextpiece = event.piece;
				drawMe=true;
			break;
			case 7:
				field.text.setCountdown(event.pending);
				if (options.sound) {
					if (event.pending)
						resources.sounds.startBeep1();
					else
						resources.sounds.startBeep2();
				}
				drawMe=true;
			break;
		}
	}
	field.text.setBpm(bpmCounter.calcBpm(currentTime));
	sf::Int16 timer = recorder.comboTimer-(((currentTime-recorder.comboSet).asMilliseconds()/6.0)/10.0);
	if (timer<0)
		timer=0;
	if (timer != recorder.lastComboTimer) {
		field.text.setComboTimer(timer);
		recorder.lastComboTimer = timer;
		drawMe=true;
	}
	return false;
}

void gamePlay::updateReplayText(RecordingEvent& event) {
	field.text.setCombo(event.combo);
	field.text.setPending(event.pending);

	field.text.setComboTimer(event.comboTimer);

	if (event.combo != recorder.prevCombo) {
		if (options.sound)
			playComboSound(event.combo);
		recorder.comboSet = event.time;
		recorder.comboTimer = event.comboTimer;
	}
	recorder.prevCombo = event.combo;
}