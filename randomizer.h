#ifndef RANDOMIZER_H
#define RANDOMIZER_H

#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int_distribution.hpp>

class randomizer {
public:
	randomizer() : dist(0, 9) { reset(); }
	short noP[7];
	short total;

	float cogP[7];

	boost::random::mt19937 gen;
    boost::random::uniform_int_distribution<> dist;

    short getHole();
    void seedHole(short seedNr);
	short getPiece();
	void reset();
};

#endif