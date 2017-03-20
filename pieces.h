#ifndef PIECES_H
#define PIECES_H

#define SFML_NO_DEPRECATED_WARNINGS
#include <SFML/Graphics.hpp>

class basePieces {
public:
	sf::Uint8 grid[4][4];

	sf::Uint8 rotation;

	short posX;
	short posY;

	short lpiece;

	sf::Uint8 tile;

	void rcw();
	void rccw();

	void mleft() { posX--; }
	void mright() { posX++; }
	void mup() { posY--; }
	void mdown() { posY++; }
};

#endif