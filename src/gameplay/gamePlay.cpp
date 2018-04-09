#include "gamePlay.h"
#include "gameField.h"
#include "optionSet.h"
#include "pieces.h"
#include "randomizer.h"
#include "textures.h"
#include "FieldBackMaker.h"
#include "GameSignals.h"
#include "Resources.h"
#include "packetcompress.h"
#include <SFML/Graphics.hpp>
#include <iostream>
#include <deque>
#include <string>
using std::to_string;
using std::cout;
using std::endl;

#define PI 3.14159265

static auto& IsVisible = Signal<bool, int>::get("IsVisible");
static auto& UpdateChallengesUI = Signal<void, GameplayData&>::get("UpdateChallengesUI");
static auto& PlaySound = Signal<void, int>::get("PlaySound");
static auto& Cheese30L = Signal<bool>::get("Cheese30L");
static auto& SeedRander = Signal<void, int, int>::get("SeedRander");
static auto& SetGameState = Signal<void, GameStates>::get("SetGameState");
static auto& SetRoundlenghtForScore = Signal<void, int>::get("SetRoundlenghtForScore");
static auto& AddLocalScore = Signal<void, GameplayData&, uint16_t, const sf::String&, uint16_t>::get("AddLocalScore");
static auto& GetName = Signal<const sf::String&>::get("GetName");
static auto& UpdateReplayUI = Signal<void, sf::Time>::get("UpdateReplayUI");
static auto& GetRecTime = Signal<sf::Time>::get("GetRecTime");

gamePlay::gamePlay(Resources& _resources) :
field(_resources),
options(*_resources.options),
resources(_resources),
garbage(data.linesBlocked),
combo(data.maxCombo),
dataSender(*this),
aiManager(gameclock),
state(std::unique_ptr<GPBaseState>(new GPMainMenu(*this))),
showPressEnterText(true)
{
	resources.compressor->game = this;
	nextpiece=0;

	lockdown=false;

	rKey=false;
	lKey=false;
	dKey=false;

	field.text.setName(options.name);
	pressEnterText.setFont(resources.gfx->typewriter);
	pressEnterText.setCharacterSize(17);
	pressEnterText.setColor(sf::Color::White);
	pressEnterText.setPosition(10,500);
	pressEnterText.setString("press P to start practice");

	updateBasePieces();

	field.backgroundTexture = makeBackground(options.fieldVLines, options.fieldHLines, options.lineStyle, options.lineColor);
    field.background.setTexture(field.backgroundTexture);
    field.background.setPosition(5,5);

    connectSignal("Ready", &gamePlay::ready, this);
    connectSignal("Away", &gamePlay::away, this);
    connectSignal("SetAway", &gamePlay::setAway, this);
    connectSignal("SetGameBackColor", &gamePlay::setBackgroundColor, this);
    connectSignal("SetDrawMe", &gamePlay::setDrawMe, this);
    connectSignal("MakeBackgroundLines", &gamePlay::makeBackgroundLines, this);
    connectSignal("UpdateGamePieces", &gamePlay::updateBasePieces, this);
    connectSignal("StartCountDown", &gamePlay::startCountdown, this);
    connectSignal("GetName", [&]() -> const sf::String& { return field.text.name; });
    connectSignal("SetName", &gamePlay::setName, this);
    connectSignal("RecUpdateScreen", &gamePlay::updateReplayScreen, this);
    connectSignal("GetGameData", [&]() -> GameplayData& { return data; });
    connectSignal("GetGameTime", [&](){ return gameclock.getElapsedTime(); });
    connectSignal("GameOver", &gamePlay::gameOver, this);
    connectSignal("PushGarbage", &gamePlay::pushGarbage, this);
    connectSignal("GameClear", [&](){ field.clear(); });
    connectSignal("GameDraw", &gamePlay::draw, this);
    connectSignal("GameSetup", &gamePlay::startSetup, this);
    connectSignal("AddGarbage", &gamePlay::addGarbage, this);
    connectSignal("SetGameState", [&](GameStates newState){ GPBaseState::set(state, newState); });
    connectSignal("MakeDrawCopy", &gamePlay::makeDrawCopy, this);
    connectSignal("GameDrawSprite", [&](){ resources.window.draw(field.sprite); });

    Net::takeSignal(9, &gamePlay::addGarbage, this);
    Net::takeSignal(13, [&](uint16_t id1, uint16_t id2){
		if (id1 == resources.myId) {
			field.text.setPosition(id2);
			drawMe=true;
		}
	});
}

gamePlay::~gamePlay() {}

void gamePlay::startGame() {
	recorder.start(field.square);
	makeNewPiece();
	drawMe=true;
	garbage.clear();
	gameclock.restart();
	combo.clear();
	pieceDropDelay.clear();
	data.clear();
	autoaway=true;
	lockdown=false;
	bpmCounter.clear();
	aiManager.startRound();
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
			pieceDropDelay.reset(gameclock.getElapsedTime());
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
	pieceDropDelay.reset(gameclock.getElapsedTime());
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
	data.pieceCount++;
	bpmCounter.addPiece(_time);
}

void gamePlay::makeNewPiece() {
	copyPiece(nextpiece);

	nextpiece = rander.getPiece();
	lockdown=false;
	if (!field.possible()) {
		addPiece(gameclock.getElapsedTime());
		gameOver(0);
	}

	if (recorder.rec) {
		addRecEvent(1, 0);
		addRecEvent(6, 0);
	}
}

void gamePlay::copyPiece(uint8_t np) {
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
	if (field.status == 0)
		return;
	field.drawField(options.fieldVLines | options.fieldHLines);
	drawNextPiece();
	if (showPressEnterText)
		field.texture.draw(pressEnterText);
    field.texture.display();
    drawMe=false;
}

void gamePlay::makeDrawCopy() {
	nextpieceCopy = nextpiece;
    field.squareCopy = field.square;
    field.pieceCopy = field.piece;
    drawMe=false;
    field.status = 1;
}

void gamePlay::delayCheck() {
	if (pieceDropDelay.check(gameclock.getElapsedTime())) {
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
			pieceDropDelay.reset(gameclock.getElapsedTime());
		}
	}

	uint16_t comboLinesSent = combo.check(gameclock.getElapsedTime());
	if (comboLinesSent) {
		comboLinesSent = garbage.block(comboLinesSent, gameclock.getElapsedTime(), false);
		field.text.setPending(garbage.count());
		data.linesSent += comboLinesSent;
		if (comboLinesSent) {
			if (resources.gamestate == GameStates::Game)
				SendSignal(2, comboLinesSent);
			aiManager.distributeLines(0, comboLinesSent);
		}
		drawMe=true;
	}

	uint16_t newbpm = bpmCounter.calcBpm(gameclock.getElapsedTime());
	if (newbpm != data.bpm) {
		field.text.setBpm(newbpm);
		data.bpm = newbpm;
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

	if (IsVisible(8))
		UpdateChallengesUI(data);

	field.offset = garbage.getOffset(gameclock.getElapsedTime());
	if (field.offset)
		drawMe = true;

	dataSender.sendstate();

	if (aiManager.update(gameclock.getElapsedTime()))
		gameOver(1);
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
	data.garbageCleared+=lines.y;
	if (lines.y)
		if (resources.gamestate == GameStates::Game)
			SendSignal(3, lines.y);
	data.linesCleared+=lines.x;
	if (lines.x==0) {
		combo.noClear();
		PlaySound(0);
		return;
	}
	uint16_t amount = garbage.block(lines.x-1, gameclock.getElapsedTime());
	data.linesSent += amount;
	if (amount) {
		if (resources.gamestate == GameStates::Game)
			SendSignal(2, amount);
		aiManager.distributeLines(0, amount);
	}
	field.text.setPending(garbage.count());
	combo.increase(gameclock.getElapsedTime(), lines.x);

	PlaySound(1);
	playComboSound(combo.comboCount);

	setComboTimer();
	field.text.setCombo(combo.comboCount);
	
	if (recorder.rec)
		addRecEvent(5, 0);
}

void gamePlay::playComboSound(uint8_t combo) {
	if (combo==5)
		PlaySound(6);
	else if (combo==8)
		PlaySound(7);
	else if (combo==10)
		PlaySound(8);
	else if (combo==12)
		PlaySound(9);
	else if (combo==14)
		PlaySound(10);
	else if (combo==16)
		PlaySound(11);
	else if (combo==18)
		PlaySound(12);
	else if (combo==20)
		PlaySound(13);
}

void gamePlay::addGarbage(int amount) {
	garbage.add(amount, gameclock.getElapsedTime());

	data.linesRecieved+=amount;

	field.text.setPending(garbage.count());

	if (recorder.rec)
		addRecEvent(5, 0);

	PlaySound(2);
}

void gamePlay::pushGarbage() {
	garbage.setOffset(gameclock.getElapsedTime());
	field.text.setPending(garbage.count());

	uint8_t hole = (Cheese30L() ? rander.getHole(true) : rander.getHole());
	addGarbageLine(hole);

	if (recorder.rec)
		addRecEvent(4, hole);

	if (field.piece.posY > 0)
		field.piece.mup();

	if (!field.possible()) {
		if (field.piece.posY > 0)
			field.piece.mup();
		else
			gameOver(0);
		if (!lockdown)
			lockDownTime=gameclock.getElapsedTime()+sf::milliseconds(400);
		lockdown=true;
		if (recorder.rec)
			addRecEvent(1, 0);
	}
}

void gamePlay::addGarbageLine() {
	uint8_t hole = rander.getHole();
	addGarbageLine(hole);

	if (recorder.rec)
		addRecEvent(4, hole);
}

void gamePlay::addGarbageLine(uint8_t hole) {
	for (int y=0; y<21; y++)
		for (int x=0; x<10; x++)
			field.square[y][x]=field.square[y+1][x];
	for (int x=0; x<10; x++)
		field.square[21][x]=8;
	field.square[21][hole]=0;
}

bool gamePlay::setComboTimer() {
	uint8_t count = combo.timerCount(gameclock.getElapsedTime());
	return field.text.setComboTimer(count);
}

void gamePlay::startCountdown() {
	countDownTime = sf::seconds(0);
	countDowncount = 3;

	if (!aiManager.empty())
		SeedRander(rander.uniqueRnd()*100000, rander.uniqueRnd()*100000);

	field.clear();
	makeNewPiece();
	while (nextpiece == 2 || nextpiece == 3) {
		rander.reset();
		makeNewPiece();
	}
	aiManager.startCountdown();
	field.piece.piece=7;
	combo.clear();
	lKeyTime=sf::seconds(0);
	rKeyTime=sf::seconds(0);
	garbage.clear();
	dataSender.reset();
	countDown(3);
	if (recorder.rec)
		addRecEvent(7, 3);
}

bool gamePlay::countDown() {
	if (gameclock.getElapsedTime()>countDownTime) {
		countDownTime = gameclock.getElapsedTime() + sf::seconds(1);
		countDowncount--;
		field.text.setCountdown(countDowncount);
		aiManager.countDown(countDowncount);
		if (recorder.rec)
			addRecEvent(7, countDowncount);
		if (countDowncount) {
			PlaySound(14);
			drawMe=true;
		}
		else {
			PlaySound(15);
			return true;
		}
	}
	return false;
}

bool gamePlay::countDown(short c) {
	if (c==255)
		return false;
	field.text.setCountdown(c);
	(c ? PlaySound(14) : PlaySound(15));
	drawMe=true;
	dataSender.state();
	if (recorder.rec)
		addRecEvent(7, c);
	if (c)
		return false;
	else
		return true;
}

void gamePlay::startSetup(int type) {
	if (type == 1)
		for (int i=0; i<9; i++)
			addGarbageLine(rander.getHole(true));
	else if (type == 2)
		for (int i=0; i<6; i++)
			addGarbageLine(rander.getHole(true));
	drawMe=true;
}

void gamePlay::gameOver(int winner) {
	if (resources.gamestate == GameStates::GameOver) {
		if (winner)
			dataSender.gameover(winner);
		return;
	}
	data.roundDuration = gameclock.getElapsedTime().asMilliseconds();
	data.bpm = (int)(data.pieceCount / ((float)gameclock.getElapsedTime().asSeconds()) * 60.0);
	field.text.setBpm(data.bpm);
	field.text.setCombo(data.maxCombo);

	addRecEvent(5, 0);
	recorder.stop();

    if (winner) {
    	field.text.setGameover(2);
    	autoaway = false;
    }
    else
		field.text.setGameover(1);

	dataSender.gameover(winner);
	SetGameState(GameStates::GameOver);
	if (!resources.playonline) {
		SetRoundlenghtForScore(gameclock.getElapsedTime().asSeconds());
		if (aiManager.empty())
			AddLocalScore(data, 0, GetName(), 0);
		else
			aiManager.setScore(data);
	}
	drawMe=true;
}

void gamePlay::away() {
	resources.away = !resources.away;
	setAway(resources.away);
}

void gamePlay::setAway(bool away) {
	if (away) {
		resources.away=true;
		SendSignal(5);
		gameOver(0);
		field.text.away=true;
		autoaway=false;
		drawMe=true;
	}
	else {
		resources.away=false;
		autoaway=false;
		SendSignal(6);
		field.text.away=false;
		drawMe=true;
	}
}

void gamePlay::ready() {
	if (resources.gamestate == GameStates::GameOver) {
		if (field.text.ready) {
			SendSignal(8);
			field.text.ready=false;
		}
		else {
			SendSignal(7);
			field.text.ready=true;
		}
		drawMe=true;
	}
}

void gamePlay::drawNextPiece() {
	for (int y=0; y<4; y++)
        for (int x=0; x<4; x++)
            if (basepiece[nextpiece].grid[y][x] != 0) {
                	field.tile[basepiece[nextpiece].tile-1].setPosition(sf::Vector2f(-15*basepiece[nextpiece].lpiece+330+x*30, 45+y*30));
                	field.texture.draw(field.tile[basepiece[nextpiece].tile-1]);
            	}
}

void gamePlay::addRecEvent(uint8_t type, uint8_t value) {
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
				data.clear();
				field.clear();
				garbage.setOffset(sf::seconds(-2));
				for (int y=0; y<22; y++)
					for (int x=0; x<10; x++)
						field.square[y][x] = recorder.starting_position[y][x];
				bpmCounter.clear();
				recorder.comboSet=sf::seconds(0);
				recorder.lastComboTimer=10;
				recorder.comboTimer=0;
			break;
			case 101:
				recorder.halt=true;
				drawMe=true;
				field.text.setBpm((int)(data.pieceCount / ((float)(event.time.asSeconds()))*60.0));
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
				data.linesCleared+=lines.x;
				data.garbageCleared+=lines.y;
				(lines.x ? PlaySound(1) : PlaySound(0));
				drawMe = true;
			}
			break;
			case 4:
				addGarbageLine(event.x);
				garbage.setOffset(event.time);
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
				(event.pending ? PlaySound(14) : PlaySound(15));
				drawMe=true;
			break;
		}
	}
	field.text.setBpm(bpmCounter.calcBpm(currentTime));
	int16_t timer = recorder.comboTimer-(((currentTime-recorder.comboSet).asMilliseconds()/6.0)/10.0);
	if (timer<0)
		timer=0;
	if (timer != recorder.lastComboTimer) {
		field.text.setComboTimer(timer);
		recorder.lastComboTimer = timer;
		drawMe=true;
	}
	UpdateReplayUI(GetRecTime());
	if (IsVisible(8))
		UpdateChallengesUI(data);
	field.offset = garbage.getOffset(currentTime);
	if (field.offset)
		drawMe = true;
	return false;
}

void gamePlay::updateReplayText(RecordingEvent& event) {
	field.text.setCombo(event.combo);
	field.text.setPending(event.pending);

	field.text.setComboTimer(event.comboTimer);

	if (event.combo != recorder.prevCombo) {
		playComboSound(event.combo);
		recorder.comboSet = event.time;
		recorder.comboTimer = event.comboTimer;
	}
	recorder.prevCombo = event.combo;
}

void gamePlay::setBackgroundColor(int val) {
	field.setBackColor(val);
}

void gamePlay::setDrawMe() {
	drawMe=true;
}

void gamePlay::makeBackgroundLines() {
	field.backgroundTexture = makeBackground(resources.options->fieldVLines, resources.options->fieldHLines, resources.options->lineStyle, resources.options->lineColor);
}

void gamePlay::setName(const sf::String& name) {
	field.text.setName(name);
}

void gamePlay::updateReplayScreen() {
	playReplay();
	drawMe=true;
}