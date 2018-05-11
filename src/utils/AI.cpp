#include "AI.h"
#include "Options.h"
#include "Resources.h"
#include <fstream>

AI::AI(obsField& _field, sf::Clock& _gameclock) :
resources(_field.resources),
field(&_field),
firstMove(resources),
secondMove(resources),
basepiece(Options::get<std::array<basePieces, 7>>("BasePieces")),
garbage(data.linesBlocked),
combo(data.maxCombo),
gameclock(_gameclock) {
	movingPiece=false;
	nextmoveTime=sf::seconds(0);
	movepieceTime=sf::seconds(0);
	updateField=0;
	setPiece(0);

	downstackWeights[0] = -0.777562;
	downstackWeights[1] = -0.957217;
	downstackWeights[2] = -0.206355;
	downstackWeights[3] = 0.305608;
	downstackWeights[4] = -0.0985396;
	downstackWeights[5] = -0.571009;
	downstackWeights[6] = -0.0826352;
	downstackWeights[7] = -0.268683;
	downstackWeights[8] = 0.01;
	downstackWeights[9] = -0.947217;

	stackWeights[0] = 0.0646257;
	stackWeights[1] = -0.781367;
	stackWeights[2] = -0.079562;
	stackWeights[3] = -0.112896;
	stackWeights[4] = 0.238397;
	stackWeights[5] = -0.136575;
	stackWeights[6] = -0.0488756;
	stackWeights[7] = -0.206737;
	stackWeights[8] = 0.01;
	stackWeights[9] = -0.771367;
}

void AI::setField(obsField& _field) {
	field = &_field;
}

void AI::startMove() {
	movingPiece=true;
	//movepieceTime = gameclock.getElapsedTime()-sf::microseconds(1);
	movepieceTime = nextmoveTime - moveTime;
	currentMove.clear();
	int rotationValue = firstMove.move.rot - basepiece[firstMove.piece.piece].rotation;
	if (rotationValue < 0)
		rotationValue += 4;
	if (rotationValue)
		currentMove.push_back(240+rotationValue);

	if (firstMove.move.use_path) {
		if (firstMove.move.posX > field->piece.posX) for (int i=0; i < firstMove.move.posX-field->piece.posX; i++)
			currentMove.push_back(255);
		else for (int i=0; i < field->piece.posX-firstMove.move.posX; i++)
			currentMove.push_back(254);
		for (auto it = firstMove.move.path.rbegin(); it != firstMove.move.path.rend(); it++) {
			if (*it < 240) {
				for (int i = 0; i < *it; i++)
					currentMove.push_back(252);
			}
			else currentMove.push_back(*it);
		}
	}
	else {
		if (firstMove.move.posX > field->piece.posX) for (int i=0; i < firstMove.move.posX-field->piece.posX; i++)
			currentMove.push_back(255);
		else for (int i=0; i < field->piece.posX-firstMove.move.posX; i++)
			currentMove.push_back(254);
		currentMove.push_back(253);
	}
	moveIterator = currentMove.begin();
	continueMove();
}

void AI::continueMove() {
	if (gameclock.getElapsedTime() <= movepieceTime)
		return;

	std::lock_guard<std::mutex> guard(moveQueueMutex);

	while (movingPiece && gameclock.getElapsedTime() > movepieceTime) {
		moveQueue.push_back(*moveIterator);

		if (*moveIterator == 252)
			movepieceTime += sf::milliseconds(finesseTime.asMilliseconds() / 3.0);
		else
			movepieceTime += finesseTime;
		moveIterator++;

		if (moveIterator == currentMove.end()) {
			moveQueue.push_back(230);
			movingPiece=false;

			if (firstMove.totalHeight > 125)
				setMode(Mode::Downstack);
			else if (firstMove.totalHeight < 15)
				setMode(Mode::Stack);
		}
	}
}

bool AI::executeMove() {
	std::lock_guard<std::mutex> guard(moveQueueMutex);

	while (!moveQueue.empty()) {
		if (moveQueue.front() == 255)
			field->mRight();
		else if (moveQueue.front() == 254)
			field->mLeft();
		else if (moveQueue.front() == 253) {
			field->hd();
			adjustDownMove=false;
		}
		else if (moveQueue.front() == 252) {
			if (adjustDownMove)
				adjustDownMove=false;
			else if (field->mDown())
				pieceDropDelay.reset(gameclock.getElapsedTime());
		}
		else if (moveQueue.front() == 241)
			field->rcw();
		else if (moveQueue.front() == 242)
			field->r180();
		else if (moveQueue.front() == 243)
			field->rccw();
		else if (moveQueue.front() == 230) {
			field->hd();
			field->addPiece();

			sendLines(field->clearlines(), gameclock.getElapsedTime());
			data.pieceCount++;

			setPiece(field->nextpiece);
			setNextPiece(rander.getPiece());

			bpmCounter.addPiece(gameclock.getElapsedTime());

			if (!field->possible()) {
				field->drawMe=true;
				return true;
			}
		}

		moveQueue.pop_front();

		field->drawMe=true;
	}

	return false;
}

void AI::setPiece(int piece) {
	field->piece.piece = piece;
	field->piece.tile = basepiece[piece].tile;
	field->piece.rotation = basepiece[piece].rotation;
	field->piece.posX = 3;
	field->piece.posY = 0;

	field->updatePiece();
}

void AI::setNextPiece(int piece) {
	field->nextpiece = piece;
	field->nprot = basepiece[piece].rotation;
	field->npcol = basepiece[piece].tile;
	field->npPiece = basepiece[piece];
	while (field->npPiece.current_rotation != field->nprot)
		field->npPiece.rcw();
}

void AI::startAI() {
	data.clear();
	gameCount=0;
	field->clear();
}

void AI::restartGame() {
	field->clear();
	gameCount++;
	data.clear();
	setMode(Mode::Stack);
	setPiece(0);
	field->piece.piece = 7;
	setNextPiece(rander.getPiece());
	while (field->nextpiece == 2 || field->nextpiece == 3)
		setNextPiece(rander.getPiece());
}

void AI::addGarbageLine() {
	for (int y=0; y<21; y++)
		for (int x=0; x<10; x++)
			field->square[y][x]=field->square[y+1][x];
	for (int x=0; x<10; x++)
		field->square[21][x]=8;
	field->square[21][rander.getHole()]=0;
}

void AI::setMode(Mode _mode, bool vary) {
	mode = _mode;
	if (mode == Mode::Downstack)
		weights = downstackWeights;

	else if (mode == Mode::Stack)
		weights = stackWeights;

	if (vary)
		for (int i=0; i<10; i++)
			weights[i] += rander.uniqueRnd()*0.2 - 0.1;

	firstMove.weights = weights;
	secondMove.weights = weights;
}

void AI::setSpeed(uint16_t _speed) {
	float speed = 60000000.0 / (_speed*1.05);
	moveTime = sf::microseconds(speed);
	finesseTime = sf::microseconds(speed / 15.0);
}

bool AI::playAI() {
	if (executeMove()) {
		alive = false;
		return true;
	}

	if (updateField == 1) {
		firstMove.square = field->square;
		firstMove.setPiece(field->piece.piece);
		nextpiece = field->nextpiece;
		updateField = 2;
	}

	return false;
}

void AI::aiThreadRun() {
	if (movingPiece)
		continueMove();
	else if (updateField == 2) {
		updateField=0;

		firstMove.calcHeightsAndHoles();
		if (data.pieceCount < 5)
			setMode(Mode::Stack, true);
		else if (data.pieceCount == 5)
			setMode(Mode::Stack);
		else if (firstMove.totalHeight > 130 || firstMove.highestPoint > 17)
			setMode(Mode::Downstack);
		else if (firstMove.totalHeight < 15)
			setMode(Mode::Stack);

		firstMove.calcHolesBeforePiece();
		float pieceAdjust = (data.pieceCount < 5 ? rander.uniqueRnd() * 0.5 - 0.25 : 0);
		firstMove.tryAllMoves(secondMove, nextpiece, pieceAdjust);
		startMove();
	}
	else if (!updateField && gameclock.getElapsedTime() > nextmoveTime) {
		updateField = 1;
		nextmoveTime += moveTime;
	}
}

void AI::startRound() {
	garbage.clear();
	combo.clear();
	bpmCounter.clear();
	pieceDropDelay.clear();
	incomingLines=0;
	setPiece(field->nextpiece);
	setNextPiece(rander.getPiece());
	movepieceTime = sf::seconds(0);
	nextmoveTime = sf::seconds(0);
	alive=true;
	updateField=0;
	adjustDownMove=false;
}

void AI::startCountdown() {
	restartGame();
	garbage.clear();
	combo.clear();
	bpmCounter.clear();
}

void AI::countDown(int count) {
	field->text.setCountdown(count);
	field->drawMe=true;
}

void AI::endRound(const sf::Time& _time, bool winner) {
	alive = false;
	data.bpm = data.pieceCount / _time.asSeconds() * 60.0;
	field->text.setBpm(data.bpm);
	field->text.setCombo(data.maxCombo);
	if (winner)
		field->text.setGameover(2);
	else
		field->text.setGameover(1);
	field->drawMe=true;
}

static auto& DistributeLinesLocally = Signal<void, int, int>::get("DistributeLinesLocally");

void AI::delayCheck(const sf::Time& t) {
	if (pieceDropDelay.check(t)) {
		if (field->mDown()) {
			adjustDownMove=true;
			drawMe=true;
			//lockdown=false;
		}
		/*else {
			if (!lockdown)
				lockDownTime=gameclock.getElapsedTime()+sf::milliseconds(400);
			lockdown=true;
		}*/
	}

	uint16_t comboLinesSent = combo.check(t);
	if (comboLinesSent) {
		comboLinesSent = garbage.block(comboLinesSent, t, false);
		field->text.setPending(garbage.count());
		data.linesSent += comboLinesSent;
		if (comboLinesSent)
			DistributeLinesLocally(id, comboLinesSent);
		drawMe=true;
	}

	uint16_t newbpm = bpmCounter.calcBpm(t);
	if (newbpm != data.bpm) {
		field->text.setBpm(newbpm);
		data.bpm = newbpm;
		drawMe=true;
	}


	if (setComboTimer(t))
		drawMe=true;

	if (garbage.check(t)) {
		addGarbageLine();
		if (!field->piece.posY)
			adjustDownMove=true;
		drawMe=true;
	}

	/*if (lockdown && gameclock.getElapsedTime() > lockDownTime) {
		if (!field->mDown()) {
			addPiece(gameclock.getElapsedTime());
			sendLines(field->clearlines());
			drawMe=true;
			makeNewPiece();
		}
		else
			lockdown=false;
	}*/

	field->offset = garbage.getOffset(t);
	if (field->offset)
		drawMe = true;
}

bool AI::setComboTimer(const sf::Time& t) {
	uint8_t count = combo.timerCount(t);
	return field->text.setComboTimer(count);
}

void AI::sendLines(sf::Vector2i lines, const sf::Time& t) {
	data.garbageCleared+=lines.y;
	data.linesCleared+=lines.x;
	if (lines.x==0) {
		combo.noClear();
		return;
	}
	uint16_t amount = garbage.block(lines.x-1, t);
	data.linesSent += amount;
	if (amount)
		DistributeLinesLocally(id, amount);
	field->text.setPending(garbage.count());
	combo.increase(t, lines.x);

	setComboTimer(t);
	field->text.setCombo(combo.comboCount);
}

void AI::addGarbage(uint16_t amount, const sf::Time& t) {
	garbage.add(amount, t);

	data.linesRecieved+=amount;

	field->text.setPending(garbage.count());
}
