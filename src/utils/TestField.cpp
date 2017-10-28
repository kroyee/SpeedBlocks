#include "TestField.h"
#include "Resources.h"
#include "optionSet.h"

#include <iostream>
using std::cout;
using std::endl;

void MoveInfo::clear() {
	posX=4;
	rot=0;
	score=-2000000000;
	use_path=false;
}

TestField::TestField(Resources& _resources) : BasicField(_resources) {}

void TestField::backup() {
	backupField = square;
}

void TestField::restore() {
	square = backupField;
}

void TestField::removePiece() {
	for (int x=0; x<4; x++)
        for (int y=0; y<4; y++)
            if (piece.grid[y][x])
                square[piece.posY+y][piece.posX+x]=0;
}

void TestField::setPiece(int _piece) {
	piece.piece = resources.options->basepiece[_piece].piece;
	piece.tile = resources.options->basepiece[_piece].tile;
	piece.rotation = resources.options->basepiece[_piece].rotation;
	piece.posX = 3;
	piece.posY = 0;

	for (int x=0; x<4; x++)
        for (int y=0; y<4; y++) {
            if (resources.options->basepiece[piece.piece].grid[y][x])
                piece.grid[y][x] = piece.tile;
            else
                piece.grid[y][x] = 0;
        }
    piece.lpiece = resources.options->basepiece[piece.piece].lpiece;
    piece.current_rotation = 0;
}

void TestField::checkForHoles(int y, int x) {
	for (int c=0; c<3; c++) {
		if (y+c > 21)
			break;
		if (!square[y+c][x])
			onTopOfHoles+=1.0/(c+1.0);
	}
}

void TestField::calcOnTopOfHoles() {
	onTopOfHoles=0;
	for (int x=0; x<4; x++) {
		if (piece.posX+x < 0 || piece.posX+x > 9)
			continue;

		bool belowPiece=false;
		for (int y=0; y<4; y++) {
			if (piece.posY+y > 21)
				break;

			if (piece.grid[y][x])
				belowPiece=true;
			else if (belowPiece) {
				if (square[piece.posY+y][piece.posX+x])
					checkForHoles(piece.posY+y+1, piece.posX+x);
				else
					break;
			}
		}
	}
}

void TestField::calcHolesBeforePiece() {
	holesBeforePiece=0;
	bool trackHoles=false;
	for (int x=0; x<10; x++) {
		for (int y=0; y<22; y++) {
			if (trackHoles) {
				if (!square[y][x])
					holesBeforePiece++;
			}
			else if (square[y][x])
				trackHoles=true;
		}
	}
}

void TestField::calcHeightsAndHoles() {
	holes=0;
	totalHeight=0;
	highestPoint=0;
	for (int x=0; x<10; x++) {
		heights[x]=0;
		for (int y=0; y<22; y++) {
			if (heights[x]) {
				if (!square[y][x])
					holes++;
			}
			else if (square[y][x]) {
				heights[x] = 22-y;
				totalHeight += heights[x];
			}
		}
		if (heights[x] > highestPoint)
			highestPoint = heights[x];
	}
	holes -= holesBeforePiece;
}

void TestField::calcBumpiness() {
	bumpiness=0;
	for (int x=0; x<9; x++) {
		int bump = heights[x]-heights[x+1];
		if (bump < 0)
			bumpiness += bump*-1;
		else
			bumpiness += bump;
	}
}

void TestField::calc2Wide() {
	well2Wide=0;
	well2Pos.x=-1;
	well2Pos.y=-1;
	if (heights[2] > heights[1] && heights[2] > heights[0]) {
		if (heights[1] > heights[0])
			well2Wide = heights[2]-heights[1];
		else
			well2Wide = heights[2]-heights[0];
		well2Pos.x = 0;
		well2Pos.y = 1;
	}

	int currentwell, lowestSide;
	for (int x=0; x<7; x++) {
		currentwell=0;
		lowestSide=0;
		if (heights[x] > heights[x+3])
			lowestSide = heights[x+3];
		else
			lowestSide = heights[x];

		if (lowestSide > heights[x+1] && lowestSide > heights[x+2]) {
			if (heights[x+1] > heights[x+2])
				currentwell = lowestSide-heights[x+1];
			else
				currentwell = lowestSide-heights[x+2];
		}

		if (currentwell > well2Wide) {
			well2Wide = currentwell;
			well2Pos.x = x+1;
			well2Pos.y = x+2;
		}
	}

	currentwell=0;
	if (heights[7] > heights[8] && heights[7] > heights[9]) {
		if (heights[8] > heights[9])
			currentwell = heights[7]-heights[8];
		else
			currentwell = heights[7]-heights[9];
	}

	if (currentwell > well2Wide) {
		well2Wide = currentwell;

		well2Pos.x = 8;
		well2Pos.y = 9;
	}
}

void TestField::calc1Wide() {
	well1Wide=0;
	if (heights[1] > heights[0])
		if (well2Pos.x != 0 && well2Pos.y != 0)
			well1Wide += heights[1] - heights[0];
	for (int x=1; x<9; x++) {
		if (heights[x-1] > heights[x] && heights[x+1] > heights[x])
			if (well2Pos.x != x && well2Pos.y != x)
			well1Wide += std::min(heights[x-1], heights[x+1]) - heights[x];
	}
	if (heights[8] > heights[9])
		if (well2Pos.x != 9 && well2Pos.y != 9)
			well1Wide += heights[8] - heights[9];
}

void TestField::calcScore() {
	auto newscore = checkScore();
	if (newscore > move.score) {
		move.score = newscore;
		move.posX = piece.posX;
		move.rot = piece.current_rotation;
		move.use_path = false;
	}
}

int32_t TestField::checkScore() {
	return totalHeight*weights[0] + holes*weights[1] + bumpiness*weights[2]
	+ totalLines*weights[3] + well2Wide*weights[4] + highestPoint*weights[5]
	+ onTopOfHoles*weights[6] + well1Wide*weights[7];
}

void TestField::calcMove(int addTotalLines) {
	hd();
	addPiece();
	calcOnTopOfHoles();
	auto lines = clearlines();
	totalLines = lines.x + addTotalLines;
	calcHeightsAndHoles();
	calcBumpiness();
	calc2Wide();
	calc1Wide();
	if (lines.x)
		restore();
	else
		removePiece();
}

void TestField::findBestMove(int addTotalLines) {
	backup();
	move.clear();
	if (piece.piece == 6) {
		for (int x=-2; x<9; x++) {
			piece.posX=x;
			piece.posY=0;
			if (!possible())
				continue;

			calcMove(addTotalLines);
			calcScore();
			findFinesseMove(addTotalLines);
		}
	}
	else if (piece.piece == 4 || piece.piece == 2 || piece.piece == 3) {
		for (int x=-2; x<9; x++) {
			for (int r=0; r<2; r++) {
				piece.posX=x;
				piece.posY=0;
				while (piece.current_rotation != r)
					piece.rcw();
				if (!possible())
					continue;

				calcMove(addTotalLines);
				calcScore();
				findFinesseMove(addTotalLines);
			}
		}
	}
	else {
		for (int x=-2; x<9; x++) {
			for (int r=0; r<4; r++) {
				piece.posX=x;
				piece.posY=0;
				while (piece.current_rotation != r)
					piece.rcw();
				if (!possible())
					continue;

				calcMove(addTotalLines);
				calcScore();
				findFinesseMove(addTotalLines);
			}
		}
	}
}

void TestField::tryAllMoves(TestField& field, uint8_t nextpiece) {
	backup();
	move.clear();
	field.holesBeforePiece = holesBeforePiece;

	if (piece.piece == 6) {
		for (int x=-2; x<9; x++) {
			piece.posX=x;
			piece.posY=0;
			if (!possible())
				continue;

			findNextMove(field, nextpiece);
		}
	}
	else if (piece.piece == 4 || piece.piece == 2 || piece.piece == 3) {
		for (int x=-2; x<9; x++) {
			for (int r=0; r<2; r++) {
				piece.posX=x;
				piece.posY=0;
				while (piece.current_rotation != r)
					piece.rcw();
				if (!possible())
					continue;

				findNextMove(field, nextpiece);
			}
		}
	}
	else {
		for (int x=-2; x<9; x++) {
			for (int r=0; r<4; r++) {
				piece.posX=x;
				piece.posY=0;
				while (piece.current_rotation != r)
					piece.rcw();
				if (!possible())
					continue;

				findNextMove(field, nextpiece);
			}
		}
	}
}

void TestField::findNextMove(TestField& field, uint8_t nextpiece) {
	hd();
	checkNextMove(field, nextpiece);

	if ((piece.posY < 5 ? field.move.score-100 : field.move.score) > move.score) {
		move.score = (piece.posY < 5 ? field.move.score-100 : field.move.score);
		move.posX=piece.posX;
		move.rot=piece.current_rotation;
		move.use_path=false;
	}

	tryAllFinesseMoves(field, nextpiece);
}

void TestField::checkNextMove(TestField& field, uint8_t nextpiece) {
	addPiece();
	auto lines = clearlines();
	field.square = square;
	if (field.piece.piece != resources.options->basepiece[nextpiece].piece)
		field.setPiece(nextpiece);
	field.findBestMove(lines.x);
	if (lines.x)
		restore();
	else
		removePiece();
}

void TestField::findFinesseMove(int addTotalLines) {
	auto pieceBackup = piece;
	for (int y=piece.posY+1; y < 21; y++) {
		piece = pieceBackup;
		piece.posY = y;
		if (possible()) {
			hd();
			y=piece.posY;
			calcMove(addTotalLines);
			auto newscore = checkScore();
			if (newscore > move.score) {
				if (finesseIsPossible())
					useFinesseMove(newscore);
			}
		}
	}
	piece = pieceBackup;
}

void TestField::useFinesseMove(int32_t newscore) {
	move = finesseMove;
	move.score = newscore;
}

bool TestField::setFinesseMove() {
	finesseMove.path = test_path;
	finesseMove.posX = piece.posX;
	finesseMove.rot = piece.current_rotation;
	finesseMove.use_path=true;
	return true;
}

void TestField::tryAllFinesseMoves(TestField& field, uint8_t nextpiece) {
	auto pieceBackup = piece;
	for (int y=piece.posY+2; y < 21; y++) {
		piece = pieceBackup;
		piece.posY = y;
		if (possible()) {
			hd();
			y=piece.posY;
			if (finesseIsPossible()) {
				checkNextMove(field, nextpiece);
				if (field.move.score > move.score)
					useFinesseMove(field.move.score);
			}
		}
	}
	piece = pieceBackup;
}

bool TestField::finesseIsPossible() {
	auto pieceBackup = piece;

	if (reverseWallkick())
		return restorePiece(pieceBackup, true);

	piece = pieceBackup;
	if (tryLeft(true))
		return restorePiece(pieceBackup, true);

	return restorePiece(pieceBackup, tryRight(true));
}

bool TestField::tryLeft(bool clearTestPath) {
	if (!mLeft())
		return false;

	if (clearTestPath)
		test_path.clear();
	test_path.push_back(255);

	auto up = moveUp();
	if (up)
		test_path.push_back(up);

	if (piece.posY == 0) {
		setFinesseMove();
		return true;
	}

	return tryLeft();
}

bool TestField::tryRight(bool clearTestPath) {
	if (!mRight())
		return false;

	if (clearTestPath)
		test_path.clear();
	test_path.push_back(254);

	auto up = moveUp();
	if (up)
		test_path.push_back(up);

	if (piece.posY == 0)
		return setFinesseMove();

	return tryRight();
}

bool TestField::tryUp(int turnVal) {
	test_path.clear();
	auto up = moveUp();

	test_path.push_back(turnVal+240);

	if (up)
		test_path.push_back(up);

	if (piece.posY == 0)
		 return setFinesseMove();

	if (tryLeft())
		return true;
	test_path.clear();
	test_path.push_back(turnVal+240);
	if (up)
		test_path.push_back(up);
	return tryRight();
}

bool TestField::reverseWallkick() {
	if (piece.piece == 6)
		return false;

	if (mRight()) { mLeft(); return false; }
	if (mLeft()) { mRight(); return false; }

	auto pieceBackup = piece;
	if (doWallKick())
		return restorePiece(pieceBackup, true);

	if (piece.piece == 2 || piece.piece == 3 || piece.piece == 4) {
		r180KeepPos();
		return restorePiece(pieceBackup, doWallKick());
	}

	return restorePiece(pieceBackup, false);
}

bool TestField::doWallKick() {
	auto pieceBackup = piece;
	int r;
	bool working;
	for (r=0; r<4; r++) {
		working=true;
		if (r == pieceBackup.current_rotation) {
			working=false;
			continue;
		}

		while (piece.current_rotation != r)
			piece.rcw();

		if (possible()) {
			int turnVal = pieceBackup.current_rotation - r;
			if (turnVal < 0)
				turnVal+=4;
			if (tryUp(turnVal))
				return restorePiece(pieceBackup, true);
			else
				return restorePiece(pieceBackup, false);
		}

		piece.posY--; if (possible()) break;
		piece.posY++; piece.posX--; if (possible()) break;
		piece.posX+=2; if (possible()) break;
		piece.posY--; piece.posX-=2; if (possible()) break;
		piece.posX+=2; if (possible()) break;
		piece.posY++; piece.posX-=3; if (possible()) break;
		piece.posX+=4; if (possible()) break;

		piece.posX-=2;

		working=false;
	}

	if (working) {
		int turnVal = pieceBackup.current_rotation - r;
		if (turnVal < 0)
			turnVal+=4;

		auto pieceBackup2 = piece;
		if (turnVal == 1)
			rcw();
		else if (turnVal == 2)
			r180();
		else if (turnVal == 3)
			rccw();
		else
			return restorePiece(pieceBackup, false);

		if (piece.posX != pieceBackup.posX || piece.posY != pieceBackup.posY)
			return restorePiece(pieceBackup, false);

		piece = pieceBackup2;

		if (tryUp(turnVal))
			return restorePiece(pieceBackup, true);

		piece = pieceBackup2;
		test_path.clear();
		test_path.push_back(240+turnVal);
		if (tryLeft())
			return restorePiece(pieceBackup, true);
		
		piece = pieceBackup2;
		if (test_path.size() > 1) {
			test_path.clear();
			test_path.push_back(240+turnVal);
		}
		
		return restorePiece(pieceBackup, tryRight());
	}

	return restorePiece(pieceBackup, false);
}

void TestField::r180KeepPos() {
	piece.rcw(); piece.rcw();
	if (piece.piece == 4) {
		if (piece.current_rotation == 0)
			piece.posX++;
		else if (piece.current_rotation == 1)
			piece.posY++;
		else if (piece.current_rotation == 2)
			piece.posX--;
		else
			piece.posY--;
	}
	if (piece.piece == 3) {
		if (piece.current_rotation == 0)
			piece.posX++;
		else if (piece.current_rotation == 1)
			piece.posY++;
		else if (piece.current_rotation == 2)
			piece.posX--;
		else
			piece.posY--;
	}
	if (piece.piece == 2) {
		if (piece.current_rotation == 0)
			piece.posX--;
		else if (piece.current_rotation == 1)
			piece.posY--;
		else if (piece.current_rotation == 2)
			piece.posX++;
		else
			piece.posY++;
	}
}

uint8_t TestField::moveUp() {
	bool doHD = false;
	piece.mdown();
	if (!possible())
		doHD=true;
	piece.mup();

	uint8_t count=0;
	do {
	    piece.mup();
	    count++;
	} while (possible());

	count--;
    piece.mdown();
    if (count && doHD)
    	return 253;
    return count;
}

bool TestField::restorePiece(basePieces p, bool returnValue) {
	piece = p;
	return returnValue;
}