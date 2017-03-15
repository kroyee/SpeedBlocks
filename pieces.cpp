#include "pieces.h"

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
}