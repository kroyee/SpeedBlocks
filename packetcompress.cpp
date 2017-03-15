#include "packetcompress.h"
#include "gamePlay.h"
#include <iostream>
using namespace std;

void PacketCompress::extract() {
	tmpcount=0;
	bitcount=0;
	sf::Uint8 counter=0;
	sf::Uint8 endy=0;
	sf::Uint8 temp=0;
	int y;
	getBits(endy, 5);
	for (int c=0; c<endy; c++) {
		for (int x=0; x<10; x++)
			field->square[21-c][x]=8;
		getBits(temp, 4);
		field->square[21-c][temp]=0;
	}
	for (int x=0; x<10; x++) {
		counter=0;
		getBits(counter, 5);
		for (y=0; y<counter; y++)
			field->square[y][x]=0;
		for (; y<22-endy; y++)
			getBits(field->square[y][x], 3);
	}
	temp=0; getBits(temp, 4); field->posX = temp-2;
	temp=0; getBits(temp, 5); field->posY = temp;
	for (int x=0; x<4; x++)
		for (y=0; y<4; y++) {
			temp=0; getBits(temp, 3);
			field->grid[y][x] = temp;
		}
	temp=0; getBits(temp, 3); field->nextpiece=temp;
	temp=0; getBits(temp, 3); field->npcol=temp;
	temp=0; getBits(temp, 3); field->nprot=temp;
}

void PacketCompress::getBits(sf::Uint8& byte, sf::Uint8 bits) {
	sf::Uint8 temp=0;
	temp = tmp[tmpcount]>>bitcount | temp;
	bitcount+=bits;
	if (bitcount>7) {
		bitcount-=8;
		tmpcount++;
		if (bitcount>0)
			temp = tmp[tmpcount]<<(bits-bitcount) | temp;
	}
	temp = temp<<(8-bits);
	temp = temp>>(8-bits);
	byte=temp;
}

void PacketCompress::compress() {
	clear();
	tmpcount=0;
	bitcount=0;
	sf::Uint8 counter = 0;
	int y, endy;
	for (endy=21; endy>=0; endy--) {
		if (game->field.square[endy][0]==8 || game->field.square[endy][1]==8)
			counter++;
		else
			break;
	}
	addBits(counter, 5);
	for (y=21; y>endy; y--)
		for (sf::Uint8 x=0; x<10; x++)
			if (game->field.square[y][x] == 0) {
				addBits(x, 4);
				break;
			}
	for (int x=0; x<10; x++) {
		counter=0;
		for (y=0; y<=endy; y++) {
			if (!game->field.square[y][x])
				counter++;
			else
				break;
		}
		addBits(counter, 5);
		for (; y<=endy; y++) {
			addBits(game->field.square[y][x], 3);
		}
	}
	sf::Uint8 posx=0, posy=0;
	posx = game->piece.posX+2; posy = game->piece.posY;
	addBits(posx, 4);
	addBits(posy, 5);
	for (int x=0; x<4; x++)
		for (y=0; y<4; y++)
			addBits(game->piece.grid[y][x], 3);
	addBits(game->nextpiece, 3);
	addBits(game->basepiece[game->nextpiece].tile, 3);
	addBits(game->basepiece[game->nextpiece].rotation, 3);
}

void PacketCompress::addBits(sf::Uint8& byte, sf::Uint8 bits) {
	tmp[tmpcount] = tmp[tmpcount] | byte<<bitcount;
	bitcount+=bits;
	if (bitcount>7) {
		bitcount-=8;
		tmpcount++;
		if (bitcount>0)
			tmp[tmpcount] = tmp[tmpcount] | byte>>(bits-bitcount);
	}
}

void PacketCompress::clear() {
	for (int x=0; x<100; x++)
			tmp[x]=0;
}