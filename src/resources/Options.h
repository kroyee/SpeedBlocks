#ifndef OPTIONS_H
#define OPTIONS_H

#include <SFML/Graphics.hpp>
#include <vector>
#include "pieces.h"

namespace Options {
    void save();
	void load();

    std::vector<short> pieceArray();
    void setPieceColor(short, uint8_t);
    void setDelay(short, std::string);

	template <typename T>
	T& get(std::string name);

	uint8_t& get_piece_rotation(int num);
	uint8_t& get_piece_color(int num);
	std::array<basePieces, 7>& get_basepieces();
	std::vector<sf::VideoMode>& get_videomodes();
}

#endif
