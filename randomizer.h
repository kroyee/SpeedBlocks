#ifndef RANDOMIZER_H
#define RANDOMIZER_H

#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int_distribution.hpp>

class randomizer {
public:
	randomizer() : hole_dist(0, 9), piece_dist(0, 999) { reset(); }
	short noP[7];
	short total;

	float cogP[7];

	boost::random::mt19937 hole_gen;
    boost::random::uniform_int_distribution<> hole_dist;

    boost::random::mt19937 piece_gen;
    boost::random::uniform_int_distribution<> piece_dist;

    short getHole();
    void seedHole(short seedNr);
    void seedPiece(short seedNr);
	short getPiece();
	void reset();
};

#endif