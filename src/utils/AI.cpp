#include "AI.h"
#include "optionSet.h"
#include "Resources.h"
#include <fstream>
#include <iostream>
using std::cout;
using std::endl;

AI::AI(Resources& _res) : resources(_res), field(_res), firstMove(_res), secondMove(_res) {
	movingPiece=false;
	nextmoveTime=sf::seconds(0);
	movepieceTime=sf::seconds(0);
	field = nullptr;
}

void AI::startMove(const sf::Time& t) {
	movingPiece=true;
	nextmoveTime = t+moveTime;
	movepieceTime = t-sf::microseconds(1);
	while (field->piece.current_rotation != firstMove.move.rot)
		field->piece.rcw();

	currentMove.clear();
	if (firstMove.move.use_path) {
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
}

bool AI::continueMove(const sf::Time& t) {
	while (movingPiece && t > movepieceTime) {
		if (*moveIterator == 255)
			field->mRight();
		else if (*moveIterator == 254)
			field->mLeft();
		else if (*moveIterator == 253)
			field->hd();
		else if (*moveIterator == 252)
			field->mDown();
		else if (*moveIterator == 241)
			field->rcw();
		else if (*moveIterator == 242)
			field->r180();
		else if (*moveIterator == 243)
			field->rccw();

		if (*moveIterator == 252)
			movepieceTime += finesseTime / 3.0;
		else
			movepieceTime += finesseTime;
		moveIterator++;

		if (moveIterator == currentMove.end()) {
			movingPiece=false;
			field->hd();
			field->addPiece();

			auto lines = field->clearlines();
			garbageCleared += lines.y;
			linesCleared += lines.x;
			moveCount++;

			setPiece(nextpiece);
			nextpiece = rander.getPiece();
			setNextPiece(nextpiece);

			if (!field->possible())
				return true;
		}
	}

	return false;
}

void AI::setPiece(int piece) {
	field->piece.piece = piece;
	field->piece.tile = resources.options->basepiece[piece].tile;
	field->piece.rotation = resources.options->basepiece[piece].rotation;
	field->piece.posX = 3;
	field->piece.posY = 0;

	field->updatePiece();
}

void setNextPiece(int piece) {
	field->nextpiece = piece;
	field->nprot = resources.options->basepiece[piece].rotation;
	field->npcol = resources.options->basepiece[piece].tile;
}

void AI::startAI(obsField & _field) {
	field = &_field;
	moveCount=0;
	garbageCleared=0;
	linesCleared=0;
	gameCount=0;
	field->clear();
}

void AI::restartGame() {
	field->clear();
	gameCount++;
	moveCount=0;
	setMode(Mode::Stack);
	setPiece(rander.getPiece());
	nextpiece = rander.getPiece();
}

void AI::addGarbageLine(uint8_t hole) {
	for (int y=0; y<21; y++)
		for (int x=0; x<10; x++)
			field->square[y][x]=field->square[y+1][x];
	for (int x=0; x<10; x++)
		field->square[21][x]=8;
	field->square[21][hole]=0;
	garbageAdded++;
}

void AI::setMode(Mode _mode) {
	mode = _mode;
	if (mode == Mode::Downstack) {
		weights[0] = -0.777562;
		weights[1] = -0.957217;
		weights[2] = -0.206355;
		weights[3] = 0.305608;
		weights[4] = -0.0985396;
		weights[5] = -0.571009;
		weights[6] = -0.0826352;
		weights[7] = -0.268683;
		firstMove.weights = weights;
		secondMove.weights = weights;
	}
	else if (mode == Mode::Stack) {
		weights[0] = 0.0646257;
		weights[1] = -0.781367;
		weights[2] = -0.079562;
		weights[3] = -0.112896;
		weights[4] = 0.238397;
		weights[5] = -0.136575;
		weights[6] = -0.0488756;
		weights[7] = -0.206737;
		firstMove.weights = weights;
		secondMove.weights = weights;
	}
}

void AI::setSpeed(const sf::Time& t) {
	moveTime = t;
	finesseTime = moveTime / 10.0;
}

void AI::playAI(const sf::Time& t) {
	if (movingPiece)
		continueMove(t);
	else if (t > nextmoveTime) {
		firstMove.square = field->square;
		firstMove.setPiece(field->piece.piece);
		firstMove.calcHolesBeforePiece();
		firstMove.tryAllMoves(secondMove, nextpiece);
		startMove(t);
		if (continueMove(t))
			return true;

		firstMove.calcHeightsAndHoles();
		if (firstMove.totalHeight > 130 || firstMove.highestPoint > 17)
			setMode(Mode::Downstack);
		else if (firstMove.totalHeight < 15)
			setMode(Mode::Stack);
	}
}