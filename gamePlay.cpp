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
using namespace std;

#define PI 3.14159265

gamePlay::gamePlay(Resources& _resources) :
field(_resources),
options(_resources.options),
resources(_resources)
{
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

	lockdown=false;

	rKey=false;
	lKey=false;
	dKey=false;

	comboTextVal=0;
	pendingTextVal=0;
	bpmTextVal=0;

	field.setName(options.name);

	comboText.setFont(resources.gfx.typewriter);
	comboText.setCharacterSize(48);
	comboText.setColor(sf::Color::White);
	comboText.setPosition(360,270);
	pendingText.setFont(resources.gfx.typewriter);
	pendingText.setCharacterSize(48);
	pendingText.setColor(sf::Color::White);
	pendingText.setPosition(360,500);
	countdownText.setFont(resources.gfx.typewriter);
	countdownText.setCharacterSize(96);
	countdownText.setColor(sf::Color::White);
	countdownText.setPosition(110,210);
	bpmText.setFont(resources.gfx.typewriter);
	bpmText.setCharacterSize(48);
	bpmText.setColor(sf::Color::White);
	bpmText.setPosition(360, 400);
	awayText.setFont(resources.gfx.typewriter);
	awayText.setCharacterSize(52);
	awayText.setColor(sf::Color::White);
	awayText.setPosition(110,110);
	positionText.setFont(resources.gfx.typewriter);
	positionText.setCharacterSize(48);
	positionText.setColor(sf::Color::White);
	positionText.setPosition(110,330);
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
	dclock.restart();
	iclock.restart();
	keyclock.restart();
	comboStart=sf::seconds(0);
	comboTime=sf::seconds(0);
	ddelay=sf::seconds(1);
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
	position=0;
	lockdown=false;
	for (int i=0; i<10; i++)
		oldbpm[i]=0;
	setComboTimer();
	comboText.setString(to_string(0));
	comboTextVal=0;
	pendingText.setString(to_string(0));
	pendingTextVal=0;
	bpmCount.clear();
}

void gamePlay::mRKey() {
	if (!rKey) {
		if (mRight())
			if (recorder.rec)
				addRecEvent(1, 0);
		rKeyTime=keyclock.getElapsedTime()+options.repeatDelay;
		lKey=false;
	}
	rKey=true;
	autoaway=false;
}

void gamePlay::mLKey() {
	if (!lKey) {
		if (mLeft())
			if (recorder.rec)
				addRecEvent(1, 0);
		lKeyTime=keyclock.getElapsedTime()+options.repeatDelay;
		rKey=false;
	}
	lKey=true;
	autoaway=false;
}

void gamePlay::mDKey() {
	if (!dKey) {
		if (mDown())
			if (recorder.rec)
				addRecEvent(1, 0);
		dKeyTime=keyclock.getElapsedTime()+options.repeatDelayDown;
	}
	dKey=true;
	autoaway=false;
}

bool gamePlay::possible() {
	for (int x=0; x<4; x++)
		for (int y=0; y<4; y++)
			if (piece.grid[y][x]) {
				if (piece.posX+x<0 || piece.posX+x>9 || piece.posY+y<0 || piece.posY+y>21)
					return false;
				if (field.square[piece.posY+y][piece.posX+x])
					return false;
			}
	return true;
}

bool gamePlay::mRight() {
	piece.mright();
	if (possible()) {
		drawMe=true;
		return true;
	}
	else {
		piece.mleft();
		return false;
	}
}

bool gamePlay::mLeft() {
	piece.mleft();
	if (possible()) {
		drawMe=true;
		return true;
	}
	else {
		piece.mright();
		return false;
	}
}

bool gamePlay::mDown() {
	piece.mdown();
	if (possible()) {
		drawMe=true;
		dclock.restart();
		lockdown=false;
		return true;
	}
	else {
		piece.mup();
		if (!lockdown)
			lockDownTime=keyclock.getElapsedTime()+sf::milliseconds(400);
		lockdown=true;
		return false;
	}
}

void gamePlay::hd() {
	while (possible()) { piece.mdown(); }
	piece.mup();
	addPiece();
	sendLines(field.clearlines());
	makeNewPiece();
	drawMe=true;
	dclock.restart();
	autoaway=false;
}

void gamePlay::rcw() {
	autoaway=false;
	piece.rcw();
	if (possible()) {
		if (recorder.rec)
			addRecEvent(1, 0);
		drawMe=true; return;
	}
	if (kickTest()) {
		drawMe=true;
		if (recorder.rec)
			addRecEvent(1, 0);
		return;
	}

	piece.posX-=2;
	piece.rccw();
}

void gamePlay::rccw() {
	autoaway=false;
	piece.rccw();
	if (possible()) {
		if (recorder.rec)
			addRecEvent(1, 0);
		drawMe=true; return;
	}
	if (kickTest()) {
		drawMe=true;
		if (recorder.rec)
			addRecEvent(1, 0);
		return;
	}

	piece.posX-=2;
	piece.rcw();
}

void gamePlay::r180() {
	autoaway=false;
	piece.rccw();
	piece.rccw();
	if (possible()) {
		if (recorder.rec)
			addRecEvent(1, 0);
		drawMe=true; return;
	}
	if (kickTest()) {
		drawMe=true;
		if (recorder.rec)
			addRecEvent(1, 0);
		return;
	}

	piece.posX-=2;
	piece.rcw();
	piece.rcw();
}

bool gamePlay::kickTest() {
	piece.posX--; if (possible()) return true;
	piece.posX+=2; if (possible()) return true;
	piece.posX--; piece.posY++; if (possible()) return true;
	piece.posX--; if (possible()) return true;
	piece.posX+=2; if (possible()) return true;
	piece.posX-=3; piece.posY--; if (possible()) return true;
	piece.posX+=4; if (possible()) return true;

	return false;
}

void gamePlay::addPiece() {
	if (recorder.rec)
		addRecEvent(2, 0);
	for (int x=0; x<4; x++)
        for (int y=0; y<4; y++)
            if (piece.grid[y][x])
                field.square[piece.posY+y][piece.posX+x]=piece.grid[y][x];
}

void gamePlay::makeNewPiece() {
	copyPiece(nextpiece);

	nextpiece = rander.getPiece();
	piece.posX = 3;
	piece.posY = 2;
	lockdown=false;
	if (!possible()) {
		addPiece();
		gameover=true;
		sendgameover=true;
	}
	pieceCount++;
	bpmCount.push_front(keyclock.getElapsedTime());

	if (recorder.rec) {
		addRecEvent(1, 0);
		addRecEvent(6, 0);
	}
}

void gamePlay::copyPiece(sf::Uint8 np) {
	for (int x=0; x<4; x++)
		for (int y=0; y<4; y++)
			piece.grid[y][x] = basepiece[np].grid[y][x];
	piece.lpiece=basepiece[np].lpiece;
	piece.tile=basepiece[np].tile;
	piece.piece=basepiece[np].piece;
	piece.current_rotation=basepiece[np].current_rotation;
}

void gamePlay::draw() {
	field.drawField();

	for (int y=0; y<4; y++)
        for (int x=0; x<4; x++)
            if (piece.grid[y][x] != 0)
            	if (piece.posY+y > 3) {
                	field.tile[piece.tile-1].setPosition(sf::Vector2f(5+(piece.posX+x)*30, 5+(piece.posY+y-4)*30));
                	field.texture.draw(field.tile[piece.tile-1]);
            	}

    if (options.ghostpiece) {
		short posY = piece.posY;
		while (possible()) { piece.mdown(); }
		piece.mup();

		for (int y=0; y<4; y++)
		    for (int x=0; x<4; x++)
		        if (piece.grid[y][x] != 0)
		        	if (piece.posY+y > 3) {
		            	field.tile[piece.tile+7].setPosition(sf::Vector2f(5+(piece.posX+x)*30, 5+(piece.posY+y-4)*30));
		            	field.texture.draw(field.tile[piece.tile+7]);
		        	}

		piece.posY = posY;
	}

	drawNextPiece();

    drawGameText();

    field.texture.display();
}

void gamePlay::preDraw() {
	field.drawField();

	drawNextPiece();

    drawGameText();
    field.texture.draw(countdownText);

    field.texture.display();
}

void gamePlay::delayCheck() {
	if (dclock.getElapsedTime() > ddelay) {
		piece.mdown();
		if (possible()) {
			drawMe=true;
			lockdown=false;
		}
		else {
			piece.mup();
			if (!lockdown)
				lockDownTime=keyclock.getElapsedTime()+sf::milliseconds(400);
			lockdown=true;
		}
		dclock.restart();
	}

	if (iclock.getElapsedTime() > idelay) {
		if (ddelay > sf::milliseconds(200))
			ddelay-=sf::milliseconds(10);
		else if (ddelay > sf::milliseconds(100))
			ddelay-=sf::milliseconds(5);
		else if (ddelay > sf::milliseconds(50))
			ddelay-=sf::milliseconds(2);
		else if (ddelay > sf::milliseconds(10))
			ddelay-=sf::milliseconds(1);
		iclock.restart();
	}

	sf::Time current = keyclock.getElapsedTime();
	bool recEvent=false;
	if (rKey) {
		while (current > rKeyTime) {
			rKeyTime+=options.repeatSpeed;
			if (!mRight())
				break;
			else
				recEvent=true;
		}
		if (recorder.rec && recEvent)
			addRecEvent(1, 0);
	}
	if (lKey) {
		recEvent=false;
		while (current > lKeyTime) {
			lKeyTime+=options.repeatSpeed;
			if (!mLeft())
				break;
			else
				recEvent=true;
		}
		if (recorder.rec && recEvent)
			addRecEvent(1, 0);
	}
	if (dKey) {
		recEvent=false;
		while (current > dKeyTime) {
			dKeyTime+=options.repeatSpeedDown;
			if (!mDown())
				break;
			else
				recEvent=true;
		}
		if (recorder.rec && recEvent)
			addRecEvent(1, 0);
	}

	if (lockdown && keyclock.getElapsedTime() > lockDownTime) {
		piece.mdown();
		if (!possible()) {
			piece.mup();
			addPiece();
			sendLines(field.clearlines());
			makeNewPiece();
			drawMe=true;
		}
		else {
			piece.mup();
			lockdown=false;
		}
	}

	if (keyclock.getElapsedTime() > comboStart+comboTime && comboCount!=0) {
		float durationMultiplyer = 1 + (float)keyclock.getElapsedTime().asSeconds() / 60.0 * 0.1;
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
			garbage.front().delay = keyclock.getElapsedTime()+sf::milliseconds(1500);
			short total=0;
			for (unsigned int x=0; x<garbage.size(); x++)
				total+=garbage[x].count;
			pendingText.setString(to_string(total));
			pendingTextVal=total;
		}

		linesSent += comboLinesSent;

		if (comboCount>maxCombo)
			maxCombo=comboCount;

		comboCount = 0;
		drawMe=true;
	}

	if (keyclock.getElapsedTime() > bpmMeasureTiming) {
		bpmMeasureTiming=keyclock.getElapsedTime()+sf::milliseconds(100);
		sf::Uint16 tmpbpm = bpm;
		while (bpmCount.size()) {
			if (keyclock.getElapsedTime()>bpmCount.back()+sf::seconds(5))
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
			bpmText.setString(to_string(bpm));
			bpmTextVal=bpm;
			drawMe=true;
		}
		oldbpmCount++;
		if (oldbpmCount==10)
			oldbpmCount=0;
	}


	if (setComboTimer())
		drawMe=true;

	if (garbage.size())
		if (keyclock.getElapsedTime() > garbage.front().delay) {
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
			garbage.front().delay = keyclock.getElapsedTime()+sf::milliseconds(1500);
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
				garbage.front().delay = keyclock.getElapsedTime()+sf::milliseconds(1500);
				blocked=true;
			}
		short total=0;
		for (unsigned int x=0; x<garbage.size(); x++)
			total+=garbage[x].count;
		if (blocked) {
			pendingText.setString(to_string(total));
			pendingTextVal=total;
		}
	}
	if (options.sound) {
		resources.sounds.lineClear();
	}
	linesSent+=lines.x;

	if (comboCount==0) {
		comboStart=keyclock.getElapsedTime();
		comboTime=sf::seconds(0);
	}
	comboCount++;
	comboTime+=sf::seconds((1.0/comboCount) + ((tmplines+1)/2.0)*(1.5/comboCount));

	if (options.sound)
		playComboSound(comboCount);

	setComboTimer();
	comboText.setString(to_string(comboCount));
	comboTextVal = comboCount;
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
	garbageClass garb(add, keyclock.getElapsedTime()+sf::milliseconds(1500));
	garbage.push_back(garb);

	linesRecieved+=add;

	short total=0;
	for (unsigned int x=0; x<garbage.size(); x++)
		total+=garbage[x].count;

	pendingText.setString(to_string(total));
	pendingTextVal=total;

	if (options.sound)
		resources.sounds.garbAdd();
}

void gamePlay::pushGarbage() {
	garbage.front().count--;
	garbage.front().delay+=sf::milliseconds(500);
	if (garbage.front().count == 0) {
		garbage.pop_front();
		garbage.front().delay=keyclock.getElapsedTime()+sf::milliseconds(500);
	}

	short total=0;
	for (unsigned int x=0; x<garbage.size(); x++)
		total+=garbage[x].count;

	pendingText.setString(to_string(total));
	pendingTextVal=total;

	sf::Uint8 hole = rander.getHole();
	addGarbageLine(hole);

	if (recorder.rec)
		addRecEvent(4, hole);

	if (!possible()) {
		piece.mup();
		if (!lockdown)
			lockDownTime=keyclock.getElapsedTime()+sf::milliseconds(400);
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
	sf::Time timeleft = comboStart + comboTime - keyclock.getElapsedTime();
	short count = (timeleft.asMilliseconds()/6.0) / 10.0;
	if (count>100)
		count=100;

	if (count<0)
		count=0;

	if (comboTimer.getPointCount() == static_cast<unsigned int>(count+2))
		return false;
	setComboTimerCount(count);
    return true;
}

void gamePlay::setComboTimerCount(sf::Uint8 count) {
	comboTimer.setPointCount(count+2);

    comboTimer.setPoint(0, sf::Vector2f(60, 60));
    for (int x=1; x<(count+2); x++)
        comboTimer.setPoint(x, sf::Vector2f(60 + 60*cos((PI*2)/100 * (x-26)), 60 + 60*sin((PI*2)/100 * (x-26) )));
}

void gamePlay::startCountdown() {
	countDownTime = sf::seconds(0);
	countDowncount = 0;

	gameover=false;
	countdownText.setString("3");
	field.clear();
	makeNewPiece();
	while (nextpiece == 2 || nextpiece == 3) {
		rander.reset();
		makeNewPiece();
	}
	comboText.setString("0");
	comboTextVal=0;
	pendingText.setString("0");
	pendingTextVal=0;
	bpmText.setString("0");
	bpmTextVal=0;
	countdownText.setPosition(130,210);
	countdownText.setCharacterSize(96);
	comboStart=sf::seconds(0);
	comboTime=sf::seconds(0);
	comboCount=0;
	position=0;
	setComboTimer();
	lKeyTime=sf::seconds(0);
	rKeyTime=sf::seconds(0);
	while (garbage.size())
		garbage.pop_front();
	preDraw();
	if (recorder.rec)
		addRecEvent(7, 3);
}

bool gamePlay::countDown() {
	if (keyclock.getElapsedTime()>countDownTime) {
		countDownTime = keyclock.getElapsedTime() + sf::seconds(1);
		countDowncount++;
		if (countDowncount<4) {
			if (countDowncount==2)
				countdownText.setString("2");
			else if (countDowncount==3)
				countdownText.setString("1");
			if (options.sound)
				resources.sounds.startBeep1();
			preDraw();
			if (recorder.rec)
				addRecEvent(7, 4-countDowncount);
			return false;
		}
		else {
			if (options.sound)
				resources.sounds.startBeep2();
			if (recorder.rec)
				addRecEvent(7, 0);
			return true;
		}
	}
	return false;
}

void gamePlay::countDown(short c) {
	gameover=false;
	countdownText.setPosition(130,210);
	countdownText.setCharacterSize(96);
	countdownText.setString(to_string(c));
	if (options.sound) {
		if (c)
			resources.sounds.startBeep1();
		else
			resources.sounds.startBeep2();
	}
	preDraw();
	if (recorder.rec)
		addRecEvent(7, c);
}

bool gamePlay::gameOver() {
	if (gameover) {
		if (comboCount>maxCombo)
			maxCombo=comboCount;
		linesPerMinute = (((float)linesSent)/((float)keyclock.getElapsedTime().asSeconds()))*60.0;
		bpm = (int)(pieceCount / ((float)(keyclock.getElapsedTime().asSeconds()))*60.0);
		bpmText.setString(to_string(bpm));
		bpmTextVal=bpm;

	    if (winner)
	    	countdownText.setString("Winner");
	    else
			countdownText.setString("Game Over");
		countdownText.setPosition(50,250);
		countdownText.setCharacterSize(48);

		drawGameOver();
		return true;
	}
	return false;
}

void gamePlay::drawGameOver() {
	field.drawField();

	drawNextPiece();

    drawText();

	field.texture.display();
}

void gamePlay::drawGameText() {
	field.texture.draw(comboTimer);
    field.texture.draw(comboText);
    field.texture.draw(pendingText);
    field.texture.draw(bpmText);
}

void gamePlay::drawText() {
	if (position == 1)
		positionText.setString("1st");
	else if (position == 2)
		positionText.setString("2nd");
	else if (position == 3)
		positionText.setString("3rd");
	else if (position == 0)
		positionText.setString("");
	else
		positionText.setString(to_string((int)position) + "th");

	field.texture.draw(positionText);
	field.texture.draw(countdownText);
	field.texture.draw(awayText);
	field.texture.draw(pressEnterText);
	drawGameText();
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
			event.piece = piece.piece;
			event.rotation = piece.current_rotation;
			event.color = piece.tile;
			event.pending = pendingTextVal;
			event.combo = comboTextVal;
			event.bpm = bpmTextVal;
			event.comboTimer = comboTimer.getPointCount()-2;
			event.x = piece.posX;
			event.y = piece.posY;
			recorder.addEvent(event);
		break;
		case 3:
		break;
		case 4:
			event.x = value;
		case 5:
			event.pending = pendingTextVal;
			event.combo = comboTextVal;
			event.bpm = bpmTextVal;
			event.comboTimer = comboTimer.getPointCount()-2;
			recorder.addEvent(event);
		break;
		case 6:
			event.piece = basepiece[nextpiece].piece;
			event.rotation = basepiece[nextpiece].current_rotation;
			event.color = basepiece[nextpiece].tile;
			event.pending = pendingTextVal;
			event.combo = comboTextVal;
			event.bpm = bpmTextVal;
			event.comboTimer = comboTimer.getPointCount()-2;
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
				while (piece.current_rotation != event.rotation)
					piece.rcw();
				piece.tile = event.color;
				piece.posX = event.x;
				piece.posY = event.y;

				updateReplayText(event);
				drawMe = true;
			break;
			case 2:
			{
				copyPiece(event.piece);
				while (piece.current_rotation != event.rotation)
					piece.rcw();
				piece.tile = event.color;
				piece.posX = event.x;
				piece.posY = event.y;
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
				countdownText.setPosition(130,210);
				countdownText.setCharacterSize(96);
				countdownText.setString(to_string(event.pending));
				if (options.sound) {
					if (event.pending)
						resources.sounds.startBeep1();
					else
						resources.sounds.startBeep2();
				}

				if (event.pending)
					preDrawMe=true;
				else
					drawMe=true;
			break;
		}
	}
	return false;
}

void gamePlay::updateReplayText(RecordingEvent& event) {
	bpmText.setString(to_string(event.bpm));
	comboText.setString(to_string(event.combo));
	pendingText.setString(to_string(event.pending));

	setComboTimerCount(event.comboTimer);

	if (event.combo != recorder.prevCombo)
		if (options.sound)
			playComboSound(event.combo);
	recorder.prevCombo = event.combo;
}