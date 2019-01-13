#ifndef PIECES_H
#define PIECES_H

#include <SFML/Graphics.hpp>
#include <array>

class basePieces {
public:
	basePieces();
	std::array<std::array<uint8_t, 4>, 4> grid;

	uint8_t rotation;
	uint8_t current_rotation;

	int8_t posX, posY;

	short lpiece;

	uint8_t tile, piece;

	void rcw();
	void rccw();

	void mleft() { posX--; }
	void mright() { posX++; }
	void mup() { posY--; }
	void mdown() { posY++; }

	void setRotation(uint8_t new_rotation);
	void setColor(uint8_t new_color);
};

#endif
