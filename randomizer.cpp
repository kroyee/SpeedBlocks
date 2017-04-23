#include "randomizer.h"

short randomizer::getPiece() {
	short returnpiece=0;
	float piece = piece_dist(piece_gen);
	for (int i=0; i<7; i++) {
		piece-=cogP[i];
		if (piece < 0) {
			returnpiece = i;
			break;
		}
	}
	noP[returnpiece]++;
	total++;

	float adjust=(cogP[returnpiece]/4)*3;
	cogP[returnpiece]-=adjust;
	adjust/=6.0;

	for (int i=0; i<7; i++)
		if (i!=returnpiece)
			cogP[i]+=adjust;

	return returnpiece;
}

void randomizer::seedPiece(short seedNr) {
	piece_gen.seed(seedNr);
}

short randomizer::getHole() {
	return hole_dist(hole_gen);
}

void randomizer::seedHole(short seedNr) {
	hole_gen.seed(seedNr);
}

void randomizer::reset() {
	for (int i=0; i<7; i++) {
		noP[i]=0;
		cogP[i]=1000/7;
		total=0;
	}
}