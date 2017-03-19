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
#include "MingwConvert.h"
using namespace std;

#define PI 3.14159265

void gamePlay::startGame() {
	field.clear();
	makeNewPiece();
	draw();
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
	for (int i=0; i<10; i++)
		oldbpm[i]=0;
	setComboTimer();
	comboText.setString(to_string(0));
	pendingText.setString(to_string(0));
	bpmCount.clear();
}

void gamePlay::mRKey() {
	if (!rKey) {
		mRight();
		rKeyTime=keyclock.getElapsedTime()+options.repeatDelay;
		lKey=false;
	}
	rKey=true;
	autoaway=false;
}

void gamePlay::mLKey() {
	if (!lKey) {
		mLeft();
		lKeyTime=keyclock.getElapsedTime()+options.repeatDelay;
		rKey=false;
	}
	lKey=true;
	autoaway=false;
}

void gamePlay::mDKey() {
	if (!dKey) {
		mDown();
		dKeyTime=keyclock.getElapsedTime()+options.repeatDelayDown;
	}
	dKey=true;
	autoaway=false;
}

bool gamePlay::possible() {
	bool possible = true;
	for (int x=0; x<4; x++)
		for (int y=0; y<4; y++)
			if (piece.grid[y][x]) {
				if (piece.posX+x<0 || piece.posX+x>9 || piece.posY+y<0 || piece.posY+y>21)
					possible = false;
				if (field.square[piece.posY+y][piece.posX+x])
					possible = false;
			}
	return possible;
}

void gamePlay::mRight() {
	piece.mright();
	if (possible())
		draw();
	else
		piece.mleft();
}

void gamePlay::mLeft() {
	piece.mleft();
	if (possible())
		draw();
	else
		piece.mright();
}

void gamePlay::mDown() {
	piece.mdown();
	if (possible()) {
		draw();
		dclock.restart();
	}
	else
		piece.mup();
}

void gamePlay::hd() {
	while (possible()) { piece.mdown(); }
	piece.mup();
	addPiece();
	sendLines(field.clearlines());
	makeNewPiece();
	draw();
	dclock.restart();
	autoaway=false;
}

void gamePlay::rcw() {
	autoaway=false;
	piece.rcw();
	if (possible()) {
		draw(); return;
	}
	piece.posX--; if (possible()) { draw(); return; }
	piece.posX+=2; if (possible()) { draw(); return; }
	piece.posX--; piece.posY++; if (possible()) { draw(); return; }
	piece.posX--; if (possible()) { draw(); return; }
	piece.posX+=2; if (possible()) { draw(); return; }
	piece.posX-=3; piece.posY--; if (possible()) { draw(); return; }
	piece.posX+=4; if (possible()) { draw(); return; }

	piece.posX-=2;
	piece.rccw();
}

void gamePlay::rccw() {
	autoaway=false;
	piece.rccw();
	if (possible()) {
		draw(); return;
	}
	piece.posX--; if (possible()) { draw(); return; }
	piece.posX+=2; if (possible()) { draw(); return; }
	piece.posX--; piece.posY++; if (possible()) { draw(); return; }
	piece.posX--; if (possible()) { draw(); return; }
	piece.posX+=2; if (possible()) { draw(); return; }
	piece.posX-=3; piece.posY--; if (possible()) { draw(); return; }
	piece.posX+=4; if (possible()) { draw(); return; }

	piece.posX-=2;
	piece.rcw();
}

void gamePlay::r180() {
	autoaway=false;
	piece.rccw();
	piece.rccw();
	if (possible()) {
		draw(); return;
	}
	piece.posX--; if (possible()) { draw(); return; }
	piece.posX+=2; if (possible()) { draw(); return; }
	piece.posX--; piece.posY++; if (possible()) { draw(); return; }
	piece.posX--; if (possible()) { draw(); return; }
	piece.posX+=2; if (possible()) { draw(); return; }
	piece.posX-=3; piece.posY--; if (possible()) { draw(); return; }
	piece.posX+=4; if (possible()) { draw(); return; }

	piece.posX-=2;
	piece.rcw();
	piece.rcw();
}

void gamePlay::addPiece() {
	for (int x=0; x<4; x++)
        for (int y=0; y<4; y++)
            if (piece.grid[y][x])
                field.square[piece.posY+y][piece.posX+x]=piece.grid[y][x];
}

void gamePlay::makeNewPiece() {
	for (int x=0; x<4; x++)
		for (int y=0; y<4; y++)
			piece.grid[y][x] = basepiece[nextpiece].grid[y][x];
	piece.lpiece=basepiece[nextpiece].lpiece;
	piece.tile=basepiece[nextpiece].tile;

	nextpiece = rander.getPiece();
	piece.posX = 3;
	piece.posY = 2;
	if (!possible()) {
		gameover=true;
		sendgameover=true;
	}
	pieceCount++;
	bpmCount.push_front(keyclock.getElapsedTime());
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

	for (int y=0; y<4; y++)
        for (int x=0; x<4; x++)
            if (basepiece[nextpiece].grid[y][x] != 0) {
                	field.tile[basepiece[nextpiece].tile-1].setPosition(sf::Vector2f(-15*basepiece[nextpiece].lpiece+330+x*30, 45+y*30));
                	field.texture.draw(field.tile[basepiece[nextpiece].tile-1]);
            	}

    field.texture.draw(comboTimer);
    field.texture.draw(comboText);
    field.texture.draw(pendingText);
    field.texture.draw(bpmText);

    field.texture.display();
}

void gamePlay::preDraw() {
	field.drawField();

	for (int y=0; y<4; y++)
        for (int x=0; x<4; x++)
            if (basepiece[nextpiece].grid[y][x] != 0) {
                	field.tile[basepiece[nextpiece].tile-1].setPosition(sf::Vector2f(-15*basepiece[nextpiece].lpiece+330+x*30, 45+y*30));
                	field.texture.draw(field.tile[basepiece[nextpiece].tile-1]);
            	}

    field.texture.draw(comboTimer);
    field.texture.draw(comboText);
    field.texture.draw(pendingText);
    field.texture.draw(countdownText);
    field.texture.draw(bpmText);

    field.texture.display();
}

void gamePlay::delayCheck() {
	if (dclock.getElapsedTime() > ddelay) {
		piece.mdown();
		if (possible()) {
			draw();
		}
		else {
			piece.mup();
			addPiece();
			sendLines(field.clearlines());
			makeNewPiece();
			draw();
		}
		dclock.restart();
	}

	if (iclock.getElapsedTime() > idelay && ddelay > sf::milliseconds(100)) {
		ddelay-=sf::milliseconds(10);
		iclock.restart();
	}

	if (rKey) {
		if (keyclock.getElapsedTime() > rKeyTime) {
			mRight();
			rKeyTime+=options.repeatSpeed;
		}
	}
	if (lKey)
		if (keyclock.getElapsedTime() > lKeyTime) {
			mLeft();
			lKeyTime+=options.repeatSpeed;
		}
	if (dKey)
		if (keyclock.getElapsedTime() > dKeyTime) {
			mDown();
			dKeyTime+=options.repeatSpeedDown;
		}

	if (keyclock.getElapsedTime() > comboStart+comboTime && comboCount!=0) {
		linesSent = linesSent + comboCount * pow(1.09, comboCount);

		cout << "Combo " << comboCount << " sent. Total " << linesSent << " SPM " << (((float)linesSent)/((float)keyclock.getElapsedTime().asSeconds()))*60.0 << endl;

		if (comboCount>maxCombo)
			maxCombo=comboCount;

		comboCount = 0;
		comboTime = sf::seconds(0);
		comboText.setString(to_string(comboCount));
		draw();
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
			draw();
		}
		oldbpmCount++;
		if (oldbpmCount==10)
			oldbpmCount=0;
	}


	if (setComboTimer())
		draw();

	if (garbage.size())
		if (keyclock.getElapsedTime() > garbage.front().delay) {
			pushGarbage();
			draw();
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
		return;
	}
	else if (lines.x==1) {
		if (garbage.size())
			garbage.front().delay = keyclock.getElapsedTime()+sf::milliseconds(1500);
	}
	else {
		lines.x--;
		for (int i=0; i<tmplines; i++)
			if (garbage.size()) {
				garbage.front().count--;
				if (garbage.front().count == 0)
					garbage.pop_front();
				lines.x--;
				linesBlocked++;
				garbage.front().delay = keyclock.getElapsedTime()+sf::milliseconds(1500);
			}
	}

	linesSent+=lines.x;

	if (comboCount==0)
		comboStart=keyclock.getElapsedTime();
	comboCount++;
	comboTime+=sf::seconds((2.0/comboCount) + ((tmplines+1)/2.0)*(2.0/comboCount));

	if (options.sound) {
		if (comboCount==5)
			sounds->combo5();
		else if (comboCount==8)
			sounds->combo8();
		else if (comboCount==11)
			sounds->combo11();
		else if (comboCount==13)
			sounds->combo13();
		else if (comboCount==15)
			sounds->combo15();
		else if (comboCount==17)
			sounds->combo17();
		else if (comboCount==19)
			sounds->combo19();
		else if (comboCount==21)
			sounds->combo21();
	}

	setComboTimer();
	comboText.setString(to_string(comboCount));
}

void gamePlay::addGarbage(short add) {
	garbageClass garb(add, keyclock.getElapsedTime()+sf::milliseconds(1500));
	garbage.push_back(garb);

	linesRecieved+=add;

	short total=0;
	for (int x=0; x<garbage.size(); x++)
		total+=garbage[x].count;

	pendingText.setString(to_string(total));

	if (options.sound)
		sounds->garbAdd();
}

void gamePlay::pushGarbage() {
	garbage.front().count--;
	garbage.front().delay+=sf::milliseconds(500);
	if (garbage.front().count == 0) {
		garbage.pop_front();
		garbage.front().delay=keyclock.getElapsedTime()+sf::milliseconds(500);
	}

	short total=0;
	for (int x=0; x<garbage.size(); x++)
		total+=garbage[x].count;

	pendingText.setString(to_string(total));

	for (int y=0; y<21; y++)
		for (int x=0; x<10; x++)
			field.square[y][x]=field.square[y+1][x];
	for (int x=0; x<10; x++)
		field.square[21][x]=8;
	field.square[21][rander.getHole()]=0;

	if (!possible()) {
		piece.mup();
		addPiece();
		sendLines(field.clearlines());
		makeNewPiece();
	}
}

bool gamePlay::setComboTimer() {
	sf::Time timeleft = comboStart + comboTime - keyclock.getElapsedTime();
	short count = (timeleft.asMilliseconds()/5.0) / 10.0;
	if (count>100)
		count=100;

	if (count<0)
		count=0;

	if (comboTimer.getPointCount() == count+2)
		return false;
	comboTimer.setPointCount(count+2);

    comboTimer.setPoint(0, sf::Vector2f(60, 60));
    for (int x=1; x<(count+2); x++)
        comboTimer.setPoint(x, sf::Vector2f(60 + 60*cos((PI*2)/100 * (x-26)), 60 + 60*sin((PI*2)/100 * (x-26) )));

    return true;
}

void gamePlay::startCountdown() {
	countDownTime = sf::seconds(0);
	countDowncount = 0;

	gameover=false;
	countdownText.setString("3");
	field.clear();
	makeNewPiece();
	comboText.setString("0");
	pendingText.setString("0");
	bpmText.setString("0");
	countdownText.setPosition(130,210);
	countdownText.setCharacterSize(96);
	comboStart=sf::seconds(0);
	comboTime=sf::seconds(0);
	comboCount=0;
	setComboTimer();
	lKeyTime=sf::seconds(0);
	rKeyTime=sf::seconds(0);
	while (garbage.size())
		garbage.pop_front();
	preDraw();
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
				sounds->startBeep1();
			preDraw();
			return false;
		}
		else {
			if (options.sound)
				sounds->startBeep2();
			return true;
		}
	}
}

void gamePlay::countDown(short c) {
	countdownText.setPosition(130,210);
	countdownText.setCharacterSize(96);
	countdownText.setString(to_string(c));
	if (options.sound) {
		if (c)
			sounds->startBeep1();
		else
			sounds->startBeep2();
	}
	preDraw();
}

bool gamePlay::gameOver() {
	if (gameover) {
		if (comboCount>maxCombo)
			maxCombo=comboCount;
		linesPerMinute = (((float)linesSent)/((float)keyclock.getElapsedTime().asSeconds()))*60.0;
		bpm = (int)(pieceCount / ((float)(keyclock.getElapsedTime().asSeconds()))*60.0);

		field.drawField();

		for (int y=0; y<4; y++)
	        for (int x=0; x<4; x++)
	            if (basepiece[nextpiece].grid[y][x] != 0) {
	                	field.tile[basepiece[nextpiece].tile-1].setPosition(sf::Vector2f(-15*basepiece[nextpiece].lpiece+330+x*30, 45+y*30));
	                	field.texture.draw(field.tile[basepiece[nextpiece].tile-1]);
	            	}

	    field.texture.draw(comboTimer);
	    field.texture.draw(comboText);
	    field.texture.draw(pendingText);

	    if (winner)
	    	countdownText.setString("Winner");
	    else
			countdownText.setString("Game Over");
		countdownText.setPosition(50,250);
		countdownText.setCharacterSize(48);
		field.texture.draw(countdownText);
		field.texture.draw(awayText);

		field.texture.display();
		return true;
	}
	return false;
}

void gamePlay::drawGameOver() {
	field.drawField();

	for (int y=0; y<4; y++)
        for (int x=0; x<4; x++)
            if (basepiece[nextpiece].grid[y][x] != 0) {
                	field.tile[basepiece[nextpiece].tile-1].setPosition(sf::Vector2f(-15*basepiece[nextpiece].lpiece+330+x*30, 45+y*30));
                	field.texture.draw(field.tile[basepiece[nextpiece].tile-1]);
            	}

    field.texture.draw(comboTimer);
    field.texture.draw(comboText);
    field.texture.draw(pendingText);
	field.texture.draw(countdownText);
	field.texture.draw(awayText);

	field.texture.display();
}