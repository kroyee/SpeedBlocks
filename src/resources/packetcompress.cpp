#include "packetcompress.h"
#include "gamePlay.h"
#include <iostream>
using std::to_string;

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
			square[21-c][x]=8;
		getBits(temp, 4);
		square[21-c][temp]=0;
	}
	for (int x=0; x<10; x++) {
		counter=0;
		getBits(counter, 5);
		for (y=0; y<counter; y++)
			square[y][x]=0;
		for (; y<22-endy; y++)
			getBits(square[y][x], 3);
	}
	getBits(temp, 4); posX = temp-2;
	getBits(temp, 5); posY = temp;
	getBits(piece, 3);
	getBits(color, 3);
	getBits(rotation, 2);
	getBits(nextpiece, 3);
	getBits(npcol, 3);
	getBits(nprot, 2);
	getBits(comboText, 5);
	getBits(pendingText, 8);
	getBits(bpmText, 8);
	getBits(comboTimerCount, 7);
	getBits(countdown, 2);
}

void PacketCompress::getBits(sf::Uint8& byte, sf::Uint8 bits) {
	byte=0;
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
	posx = game->field.piece.posX+2; posy = game->field.piece.posY;
	addBits(posx, 4);
	addBits(posy, 5);
	addBits(game->field.piece.piece, 3);
	addBits(game->field.piece.tile, 3);
	addBits(game->field.piece.current_rotation, 2);
	addBits(game->nextpiece, 3);
	addBits(game->basepiece[game->nextpiece].tile, 3);
	addBits(game->resources.options.piecerotation[game->nextpiece], 2);
	addBits(game->field.text.combo, 5);
	addBits(game->field.text.pending, 8);
	sf::Uint8 tmp;
	if (game->field.text.bpm > 255)
		tmp=255;
	else
		tmp = game->field.text.bpm;
	addBits(tmp, 8);
	tmp = game->field.text.comboTimer.getPointCount()-2;
	addBits(tmp, 7);
	tmp = game->field.text.countdown;
	addBits(tmp, 2);
	if (!countdown) {
		tmp = game->gameclock.getElapsedTime().asMilliseconds()/100;
		addBits(tmp, 8);
	}
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

void PacketCompress::copy() {
	for (int y=0; y<22; y++)
		for (int x=0; x<10; x++)
			field->square[y][x] = square[y][x];
	field->piece.posX = posX;
	field->piece.posY = posY;
	field->piece.piece = piece;
	field->piece.tile = color;
	field->piece.rotation = rotation;
	field->updatePiece();
	field->nextpiece = nextpiece;
	field->npcol = npcol;
	field->nprot = nprot;
	field->text.setBpm(bpmText);
	field->text.setPending(pendingText);
	field->text.setCombo(comboText);
	field->text.setComboTimer(comboTimerCount);
	field->text.setCountdown(countdown);
}

bool PacketCompress::validate() {
	for (int y=0; y<22; y++)
		for (int x=0; x<10; x++)
			if (square[y][x] > 8)
				return false;
	if (piece > 7)
		return false;
	if (color > 7)
		return false;
	if (rotation > 3)
		return false;
	if (posX > 8)
		return false;
	if (posY > 20)
		return false;
	if (nextpiece > 7)
		return false;
	if (npcol == 0 || npcol > 7)
		return false;
	if (nprot > 3)
		return false;
	if (countdown > 3)
		countdown = 0;

	return true;
}