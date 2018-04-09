#ifndef PACKETCOMPRESS_H
#define PACKETCOMPRESS_H

#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <deque>
class obsField;
class gamePlay;
class Recording;

class PacketCompress {
public:
	gamePlay* game;
	obsField* field;

	std::deque<uint8_t> tmp;
	uint8_t bitcount=0;
	uint16_t tmpcount=0;

	//Tempstorage for validation
	uint8_t square[22][10];
	int8_t posX, posY;
	uint8_t piece, color, rotation;
	uint8_t nextpiece, npcol, nprot;
	uint8_t comboText, pendingText, bpmText;
	uint8_t comboTimerCount;
	uint8_t countdown;

	//Tracker for replay time
	uint32_t lastTime;

	void compressReplay(Recording&, sf::Packet&);
	void dumpTmp(sf::Packet&);
	void addTimeStamp(sf::Time&);
	void extractReplay(Recording&, sf::Packet&);
	void loadTmp(sf::Packet&);
	void getTimeStamp(sf::Time&);
	void compress();
	void extract();
	void addBits(uint8_t, uint8_t);
	void getBits(uint8_t&, uint8_t);
	void clear();
	void copy();
	bool validate();
};

#endif