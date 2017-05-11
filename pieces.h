#ifndef PIECES_H
#define PIECES_H

#include <SFML/Graphics.hpp>

class basePieces {
public:
	basePieces();
	sf::Uint8 grid[4][4];

	sf::Uint8 rotation;
	sf::Uint8 current_rotation;

	sf::Int8 posX, posY;

	short lpiece;

	sf::Uint8 tile, piece;

	void rcw();
	void rccw();

	void mleft() { posX--; }
	void mright() { posX++; }
	void mup() { posY--; }
	void mdown() { posY++; }
};

#endif