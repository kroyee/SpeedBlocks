#include "packetcompress.h"
#include "gamePlay.h"
#include "Recording.h"
#include "optionSet.h"
#include <iostream>
using std::to_string;
using std::cout;
using std::endl;

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
	addBits(game->options.piecerotation[game->nextpiece], 2);
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
		sf::Uint16 timevalue = game->gameclock.getElapsedTime().asMilliseconds();
		sf::Uint8 smallpart = timevalue % 256;
		sf::Uint8 bigpart = (timevalue - smallpart) / 256;
		addBits(smallpart, 8);
		addBits(bigpart, 8);
	}
}

void PacketCompress::addBits(const sf::Uint8& byte, sf::Uint8 bits) {
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

void PacketCompress::compressReplay(Recording& replay, sf::Packet& packet) {
	clear();
	tmpcount=0;
	bitcount=0;
	sf::Uint8 counter = 0;
	int y, endy;
	for (endy=21; endy>=0; endy--) {
		if (replay.starting_position[endy][0]==8 || replay.starting_position[endy][1]==8)
			counter++;
		else
			break;
	}
	addBits(counter, 5);
	for (y=21; y>endy; y--)
		for (sf::Uint8 x=0; x<10; x++)
			if (replay.starting_position[y][x] == 0) {
				addBits(x, 4);
				break;
			}
	for (int x=0; x<10; x++) {
		counter=0;
		for (y=0; y<=endy; y++) {
			if (!replay.starting_position[y][x])
				counter++;
			else
				break;
		}
		addBits(counter, 5);
		for (; y<=endy; y++) {
			addBits(replay.starting_position[y][x], 3);
		}
	}

	dumpTmp(packet);
	for (auto&& event : replay.events) {
		switch(event.type) {
			case 100:
				addBits(0, 3); addTimeStamp(event.time);
			break;
			case 101:
				addBits(3, 3); addTimeStamp(event.time);
			break;
			case 1:
				addBits(event.type, 3); addBits(event.piece, 3); addBits(event.rotation, 2); addBits(event.color, 3);
				addBits(event.x+2, 4); addBits(event.y, 5); addTimeStamp(event.time);
			break;
			case 2:
				addBits(event.type, 3); addBits(event.piece, 3); addBits(event.rotation, 2); addBits(event.color, 3);
				addBits(event.x+2, 4); addBits(event.y, 5); addTimeStamp(event.time);
			break;
			case 4:
				addBits(event.type, 3); addBits(event.x, 4); addTimeStamp(event.time);
			break;
			case 5:
				addBits(event.type, 3); addTimeStamp(event.time);
				addBits(event.pending, 8); addBits(event.combo, 5); addBits(event.comboTimer, 7);
			break;
			case 6:
				addBits(event.type, 3); addBits(event.piece, 3); addTimeStamp(event.time);
			break;
			case 7:
				addBits(event.type, 3); addBits(event.pending, 2); addTimeStamp(event.time);
			break;
		}
		dumpTmp(packet);
	}
	if (bitcount != 0)
		packet << tmp[0];
}

void PacketCompress::dumpTmp(sf::Packet& packet) {
	for (int i=0; i<tmpcount; i++)
		packet << tmp[i];
	sf::Uint8 backup = tmp[tmpcount];
	clear();
	tmpcount=0;
	tmp[0] = backup;
}

void PacketCompress::addTimeStamp(sf::Time& stamp) {
	sf::Uint16 timevalue = stamp.asMilliseconds() % 2048;
	sf::Uint8 smallpart = timevalue % 256;
	sf::Uint8 bigpart = (timevalue - smallpart) / 256;
	addBits(smallpart, 8);
	addBits(bigpart, 3);
}

void PacketCompress::getTimeStamp(sf::Time& stamp) {
	sf::Uint32 timevalue;
	sf::Uint8 smallpart, bigpart;
	getBits(smallpart, 8);
	getBits(bigpart, 3);
	timevalue = smallpart + bigpart * 256;
	while (timevalue<lastTime)
		timevalue+=2048;
	lastTime = timevalue;
	stamp = sf::milliseconds(timevalue);
}

void PacketCompress::extractReplay(Recording& replay, sf::Packet& packet) {
	replay.clear();
	clear();
	tmpcount=100;
	bitcount=0;

	loadTmp(packet);

	sf::Uint8 counter=0;
	sf::Uint8 endy=0;
	sf::Uint8 temp=0;
	int y;
	getBits(endy, 5);
	for (int c=0; c<endy; c++) {
		for (int x=0; x<10; x++)
			replay.starting_position[21-c][x]=8;
		getBits(temp, 4);
		replay.starting_position[21-c][temp]=0;
	}
	for (int x=0; x<10; x++) {
		counter=0;
		getBits(counter, 5);
		for (y=0; y<counter; y++)
			replay.starting_position[y][x]=0;
		for (; y<22-endy; y++)
			getBits(replay.starting_position[y][x], 3);
	}

	RecordingEvent event;
	lastTime=0;
	while (true) {
		loadTmp(packet);
		getBits(event.type, 3);
		switch (event.type) {
			case 0:
				event.type=100;
				getTimeStamp(event.time);
			break;
			case 3:
				event.type=101;
				getTimeStamp(event.time);
			break;
			case 1:
				getBits(event.piece, 3); getBits(event.rotation, 2); getBits(event.color, 3);
				getBits(temp, 4); event.x=temp-2; getBits(temp, 5); event.y=temp; getTimeStamp(event.time);
			break;
			case 2:
				getBits(event.piece, 3); getBits(event.rotation, 2); getBits(event.color, 3);
				getBits(temp, 4); event.x=temp-2; getBits(temp, 5); event.y=temp; getTimeStamp(event.time);
			break;
			case 4:
				getBits(temp, 4); event.x=temp; getTimeStamp(event.time);
			break;
			case 5:
				getTimeStamp(event.time);
				getBits(event.pending, 8); getBits(event.combo, 5); getBits(event.comboTimer, 7);
			break;
			case 6:
				getBits(event.piece, 3); getTimeStamp(event.time);
			break;
			case 7:
				getBits(event.pending, 2); getTimeStamp(event.time);
			break;
			default:
			break;
		}
		replay.events.push_back(event);
		if (event.type == 101) {
			replay.duration=event.time;
			return;
		}
	}
}

void PacketCompress::loadTmp(sf::Packet& packet) {
	for (int i=tmpcount; i<100; i++)
		tmp[i-tmpcount] = tmp[i];
	for (int i=0; !packet.endOfPacket() && 100-tmpcount+i<100; i++)
		packet >> tmp[100-tmpcount+i];
	tmpcount=0;
}