#ifndef PIECES_H
#define PIECES_H

#include <SFML/Graphics.hpp>

class basePieces {
public:
	sf::Uint8 grid[4][4];

	sf::Uint8 rotation;
	sf::Uint8 current_rotation;

	short posX;
	short posY;

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