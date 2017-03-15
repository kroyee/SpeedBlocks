#ifndef PACKETCOMPRESS_H
#define PACKETCOMPRESS_H

#include <SFML/Graphics.hpp>
class obsField;
class gamePlay;

class PacketCompress {
public:
	gamePlay* game;
	obsField* field;

	sf::Uint8 tmp[100];
	sf::Uint8 bitcount=0;
	sf::Uint8 tmpcount=0;

	void compress();
	void extract();
	void addBits(sf::Uint8&, sf::Uint8);
	void getBits(sf::Uint8&, sf::Uint8);
	void clear();
};

#endif