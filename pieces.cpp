#include "pieces.h"

basePieces::basePieces() : rotation(0), current_rotation(0), posX(0), posY(0), lpiece(0), tile(1), piece(0) {}

void basePieces::rcw() {
	if (lpiece) {
		short tmpgrid[4][4];
		for (int x=0; x<4; x++)
			for (int y=0; y<4; y++)
				tmpgrid[y][x]=grid[y][x];
		for (int x=0; x<4; x++)
			for (int y=0; y<4; y++)
				grid[x][3-y]=tmpgrid[y][x];
	}
	else {
		short tmpgrid[3][3];
		for (int x=0; x<3; x++)
			for (int y=0; y<3; y++)
				tmpgrid[y][x]=grid[y][x];
		for (int x=0; x<3; x++)
			for (int y=0; y<3; y++)
				grid[x][2-y]=tmpgrid[y][x];
	}
	if (current_rotation == 3)
		current_rotation = 0;
	else
		current_rotation++;
}

void basePieces::rccw() {
	if (lpiece) {
		short tmpgrid[4][4];
		for (int x=0; x<4; x++)
			for (int y=0; y<4; y++)
				tmpgrid[y][x]=grid[y][x];
		for (int x=0; x<4; x++)
			for (int y=0; y<4; y++)
				grid[3-x][y]=tmpgrid[y][x];
	}
	else {
		short tmpgrid[3][3];
		for (int x=0; x<3; x++)
			for (int y=0; y<3; y++)
				tmpgrid[y][x]=grid[y][x];
		for (int x=0; x<3; x++)
			for (int y=0; y<3; y++)
				grid[2-x][y]=tmpgrid[y][x];
	}
	if (current_rotation == 0)
		current_rotation = 3;
	else
		current_rotation--;
}