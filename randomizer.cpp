#include "randomizer.h"
#include <SFML/Graphics.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int_distribution.hpp>

short randomizer::getPiece() {
	short returnpiece=0;
	float piece = rand()%1000;
	for (int i=0; i<7; i++) {
		piece-=cogP[i];
		if (piece < 0) {
			returnpiece = i;
			break;
		}
	}
	noP[returnpiece]++;
	total++;

	float adjust=cogP[returnpiece]/2;
	cogP[returnpiece]-=adjust;
	adjust/=6;

	for (int i=0; i<7; i++)
		if (i!=returnpiece)
			cogP[i]+=adjust;

	return returnpiece;
}

short randomizer::getHole() {
	return dist(gen);
}

void randomizer::seedHole(short seedNr) {
	gen.seed(seedNr);
}

void randomizer::reset() {
	for (int i=0; i<7; i++) {
		noP[i]=0;
		cogP[i]=1000/7;
		total=0;
	}
}