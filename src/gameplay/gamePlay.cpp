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

	comboTime=sf::seconds(0);

	comboCount=0;
	linesSent=0;
	maxCombo=0;
	linesRecieved=0;
	garbageCleared=0;

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
	field.clear();
	makeNewPiece();
	drawMe=true;
	gameclock.restart();
	comboStart=sf::seconds(0);
	comboTime=sf::seconds(0);
	dropDelay=sf::seconds(1);
	dropDelayTime=sf::seconds(0);
	bpmMeasureTiming=sf::seconds(0);
	comboCount=0;
	linesSent=0;
	linesRecieved=0;
	garbageCleared=0;
	maxCombo=0;
	linesBlocked=0;
	pieceCount=0;
	oldbpmCount=0;
	autoaway=true;
	lockdown=false;
	for (int i=0; i<10; i++)
		oldbpm[i]=0;
	bpmCount.clear();
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
	addPiece();
	sendLines(field.clearlines());
	makeNewPiece();
	dropDelayTime = gameclock.getElapsedTime();
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

void gamePlay::addPiece() {
	if (recorder.rec)
		addRecEvent(2, 0);
	field.addPiece();
}

void gamePlay::makeNewPiece() {
	copyPiece(nextpiece);

	nextpiece = rander.getPiece();
	lockdown=false;
	if (!field.possible()) {
		addPiece();
		gameover=true;
		sendgameover=true;
	}
	pieceCount++;
	bpmCount.push_front(gameclock.getElapsedTime());

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
	field.piece.posY = 2;
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

	if (lockdown && gameclock.getElapsedTime() > lockDownTime) {
		if (!field.mDown()) {
			addPiece();
			sendLines(field.clearlines());
			makeNewPiece();
			drawMe=true;
		}
		else
			lockdown=false;
	}

	if (gameclock.getElapsedTime() > comboStart+comboTime && comboCount!=0) {
		float durationMultiplyer = 1 + (float)gameclock.getElapsedTime().asSeconds() / 60.0 * 0.1;
		sf::Uint16 comboLinesSent = comboCount * pow(1.15, comboCount) * durationMultiplyer;

		bool blocked=false;
		for (int i=0; i<comboLinesSent; i++)
			if (garbage.size()) {
				garbage.front().count--;
				if (garbage.front().count == 0)
					garbage.pop_front();
				comboLinesSent--;
				linesBlocked++;
				blocked=true;
			}

		if (blocked) {
			garbage.front().delay = gameclock.getElapsedTime()+sf::milliseconds(1500);
			short total=0;
			for (unsigned int x=0; x<garbage.size(); x++)
				total+=garbage[x].count;
			field.text.setPending(total);
		}

		linesSent += comboLinesSent;

		if (comboCount>maxCombo)
			maxCombo=comboCount;

		comboCount = 0;
		drawMe=true;
	}

	if (gameclock.getElapsedTime() > bpmMeasureTiming) {
		bpmMeasureTiming=gameclock.getElapsedTime()+sf::milliseconds(100);
		sf::Uint16 tmpbpm = bpm;
		while (bpmCount.size()) {
			if (gameclock.getElapsedTime()>bpmCount.back()+sf::seconds(5))
				bpmCount.pop_back();
			else
				break;
		}
		oldbpm[oldbpmCount] = bpmCount.size()*12;
		float total=0;
		for(int i=0; i<10; i++)
			total+=oldbpm[i];
		bpm=total/10;
		if (bpm!=tmpbpm) {
			field.text.setBpm(bpm);
			drawMe=true;
		}
		oldbpmCount++;
		if (oldbpmCount==10)
			oldbpmCount=0;
	}


	if (setComboTimer())
		drawMe=true;

	if (garbage.size())
		if (gameclock.getElapsedTime() > garbage.front().delay) {
			pushGarbage();
			drawMe=true;
		}

	if (recorder.rec)
		if (recorder.timer.getElapsedTime() - recorder.events.back().time > sf::milliseconds(100))
			addRecEvent(5, 0);
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
	short tmplines=lines.x;
	if (lines.x==0) {
		comboTime-=sf::milliseconds(200);
		if (options.sound) {
			resources.sounds.pieceDrop();
		}
		return;
	}
	else if (lines.x==1) {
		if (garbage.size())
			garbage.front().delay = gameclock.getElapsedTime()+sf::milliseconds(1500);
		lines.x--;
	}
	else {
		lines.x--;
		bool blocked=false;
		for (int i=0; i<tmplines-1; i++)
			if (garbage.size()) {
				garbage.front().count--;
				if (garbage.front().count == 0)
					garbage.pop_front();
				lines.x--;
				linesBlocked++;
				garbage.front().delay = gameclock.getElapsedTime()+sf::milliseconds(1500);
				blocked=true;
			}
		short total=0;
		for (unsigned int x=0; x<garbage.size(); x++)
			total+=garbage[x].count;
		if (blocked)
			field.text.setPending(total);
	}
	if (options.sound) {
		resources.sounds.lineClear();
	}
	linesSent+=lines.x;

	if (comboCount==0) {
		comboStart=gameclock.getElapsedTime();
		comboTime=sf::seconds(0);
	}
	comboCount++;
	comboTime+=sf::seconds((1.0/comboCount) + ((tmplines+1)/2.0)*(1.5/comboCount));

	if (options.sound)
		playComboSound(comboCount);

	setComboTimer();
	field.text.setCombo(comboCount);
}

void gamePlay::playComboSound(sf::Uint8 combo) {
	if (combo==5)
		resources.sounds.combo5();
	else if (combo==8)
		resources.sounds.combo8();
	else if (combo==11)
		resources.sounds.combo11();
	else if (combo==13)
		resources.sounds.combo13();
	else if (combo==15)
		resources.sounds.combo15();
	else if (combo==17)
		resources.sounds.combo17();
	else if (combo==19)
		resources.sounds.combo19();
	else if (combo==21)
		resources.sounds.combo21();
}

void gamePlay::addGarbage(short add) {
	garbageClass garb(add, gameclock.getElapsedTime()+sf::milliseconds(1500));
	garbage.push_back(garb);

	linesRecieved+=add;

	short total=0;
	for (unsigned int x=0; x<garbage.size(); x++)
		total+=garbage[x].count;

	field.text.setPending(total);

	if (options.sound)
		resources.sounds.garbAdd();
}

void gamePlay::pushGarbage() {
	garbage.front().count--;
	garbage.front().delay+=sf::milliseconds(500);
	if (garbage.front().count == 0) {
		garbage.pop_front();
		garbage.front().delay=gameclock.getElapsedTime()+sf::milliseconds(500);
	}

	short total=0;
	for (unsigned int x=0; x<garbage.size(); x++)
		total+=garbage[x].count;

	field.text.setPending(total);

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
	sf::Time timeleft = comboStart + comboTime - gameclock.getElapsedTime();
	short count = (timeleft.asMilliseconds()/6.0) / 10.0;
	if (count>100)
		count=100;

	if (count<0)
		count=0;

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
	showPressEnterText=false;
	comboStart=sf::seconds(0);
	comboTime=sf::seconds(0);
	comboCount=0;
	lKeyTime=sf::seconds(0);
	rKeyTime=sf::seconds(0);
	while (garbage.size())
		garbage.pop_front();
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

	showPressEnterText=true;

	if (comboCount>maxCombo)
		maxCombo=comboCount;
	linesPerMinute = (((float)linesSent)/((float)gameclock.getElapsedTime().asSeconds()))*60.0;
	bpm = (int)(pieceCount / ((float)(gameclock.getElapsedTime().asSeconds()))*60.0);
	field.text.setBpm(bpm);

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
		switch (event.type) {
			case 100:
				field.clear();
			break;
			case 101:
				recorder.halt=true;
				drawMe=true;
				return false;
			break;
			case 1:
				copyPiece(event.piece);
				while (field.piece.current_rotation != event.rotation)
					field.piece.rcw();
				field.piece.tile = event.color;
				field.piece.posX = event.x;
				field.piece.posY = event.y;

				updateReplayText(event);
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
				addPiece();
				sf::Vector2i lines = field.clearlines();
				if (options.sound) {
					if (lines.x == 0)
						resources.sounds.pieceDrop();
					else
						resources.sounds.lineClear();
				}

				updateReplayText(event);
				drawMe = true;
			}
			break;
			case 4:
				addGarbageLine(event.x);

				updateReplayText(event);
				drawMe=true;
			break;
			case 5:
				updateReplayText(event);
				drawMe=true;
			break;
			case 6:
				nextpiece = event.piece;

				updateReplayText(event);
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
	return false;
}

void gamePlay::updateReplayText(RecordingEvent& event) {
	field.text.setBpm(event.bpm);
	field.text.setCombo(event.combo);
	field.text.setPending(event.pending);

	field.text.setComboTimer(event.comboTimer);

	if (event.combo != recorder.prevCombo)
		if (options.sound)
			playComboSound(event.combo);
	recorder.prevCombo = event.combo;
}