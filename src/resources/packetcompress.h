#ifndef PACKETCOMPRESS_H
#define PACKETCOMPRESS_H

#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
class obsField;
class gamePlay;
class Recording;

class PacketCompress {
public:
	gamePlay* game;
	obsField* field;

	sf::Uint8 tmp[100];
	sf::Uint8 bitcount=0;
	sf::Uint8 tmpcount=0;

	//Tempstorage for validation
	sf::Uint8 square[22][10];
	sf::Int8 posX, posY;
	sf::Uint8 piece, color, rotation;
	sf::Uint8 nextpiece, npcol, nprot;
	sf::Uint8 comboText, pendingText, bpmText;
	sf::Uint8 comboTimerCount;
	sf::Uint8 countdown;

	//Tracker for replay time
	sf::Uint32 lastTime;

	void compressReplay(Recording&, sf::Packet&);
	void dumpTmp(sf::Packet&);
	void addTimeStamp(sf::Time&);
	void extractReplay(Recording&, sf::Packet&);
	void loadTmp(sf::Packet&);
	void getTimeStamp(sf::Time&);
	void compress();
	void extract();
	void addBits(const sf::Uint8&, sf::Uint8);
	void getBits(sf::Uint8&, sf::Uint8);
	void clear();
	void copy();
	bool validate();
};

#endif