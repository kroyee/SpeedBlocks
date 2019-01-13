#include "packetcompress.h"
#include "GamePlay.h"
#include "Recording.h"
#include "Options.h"

void PacketCompress::extract() {
	tmpcount=0;
	bitcount=0;
	uint8_t counter=0;
	uint8_t endy=0;
	uint8_t temp=0;
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

void PacketCompress::getBits(uint8_t& byte, uint8_t bits) {
	byte=0;
	uint8_t temp=0;
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
	uint8_t counter = 0;
	int y, endy;
	for (endy=21; endy>=0; endy--) {
		if (game->field.square[endy][0]==8 || game->field.square[endy][1]==8)
			counter++;
		else
			break;
	}
	addBits(counter, 5);
	for (y=21; y>endy; y--)
		for (uint8_t x=0; x<10; x++)
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
	uint8_t posx=0, posy=0;
	posx = game->field.piece.posX+2; posy = game->field.piece.posY;
	addBits(posx, 4);
	addBits(posy, 5);
	addBits(game->field.piece.piece, 3);
	addBits(game->field.piece.tile, 3);
	addBits(game->field.piece.current_rotation, 2);
	addBits(game->nextpiece, 3);
	addBits(game->basepiece[game->nextpiece].tile, 3);
	addBits(Options::get<uint8_t>("piece_" + std::to_string(game->nextpiece) + "_rotation"), 2);
	addBits(game->field.text.get<FieldText::Combo>(), 5);
	addBits(game->field.text.get<FieldText::Pending>(), 8);
	uint8_t tmp;
	if (game->field.text.get<FieldText::BPM>() > 255)
		tmp=255;
	else
		tmp = game->field.text.get<FieldText::BPM>();
	addBits(tmp, 8);
	tmp = game->field.text.comboTimer.getPointCount()-2;
	addBits(tmp, 7);
	tmp = game->field.text.get<FieldText::Countdown>();
	addBits(tmp, 2);
	if (!countdown) {
		uint16_t timevalue = game->gameclock.getElapsedTime().asMilliseconds();
		uint8_t smallpart = timevalue % 256;
		uint8_t bigpart = (timevalue - smallpart) / 256;
		addBits(smallpart, 8);
		addBits(bigpart, 8);
	}
}

void PacketCompress::addBits(uint8_t byte, uint8_t bits) {
	if (tmpcount >= tmp.size())
		tmp.push_back(0);
	tmp[tmpcount] = tmp[tmpcount] | byte<<bitcount;
	bitcount+=bits;
	if (bitcount>7) {
		bitcount-=8;
		tmpcount++;
		if (bitcount>0) {
			tmp.push_back(0);
			tmp[tmpcount] = tmp[tmpcount] | byte>>(bits-bitcount);
		}
	}
}

void PacketCompress::clear() {
	tmp.clear();
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
	if (!field->text.get<FieldText::Position>())
		field->text.set<FieldText::BPM>(bpmText);
	field->text.set<FieldText::Pending>(pendingText);
	field->text.set<FieldText::Combo>(comboText);
	field->text.setComboTimer(comboTimerCount);
	if (countdown)
		field->text.set<FieldText::Countdown>(countdown);
	else
		field->text.hide<FieldText::Countdown>();
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
	uint8_t counter = 0;
	int y, endy;
	for (endy=21; endy>=0; endy--) {
		if (replay.starting_position[endy][0]==8 || replay.starting_position[endy][1]==8)
			counter++;
		else
			break;
	}
	addBits(counter, 5);
	for (y=21; y>endy; y--)
		for (uint8_t x=0; x<10; x++)
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
	}

	dumpTmp(packet);
}

void PacketCompress::dumpTmp(sf::Packet& packet) {
	for (auto i : tmp)
		packet << i;
}

void PacketCompress::addTimeStamp(sf::Time& stamp) {
	uint16_t timevalue = stamp.asMilliseconds() % 2048;
	uint8_t smallpart = timevalue % 256;
	uint8_t bigpart = (timevalue - smallpart) / 256;
	addBits(smallpart, 8);
	addBits(bigpart, 3);
}

void PacketCompress::getTimeStamp(sf::Time& stamp) {
	uint32_t timevalue;
	uint8_t smallpart, bigpart;
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

	uint8_t counter=0;
	uint8_t endy=0;
	uint8_t temp=0;
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
	tmpcount=0;
	tmp.clear();
	uint8_t temp;
	while (packet >> temp)
		tmp.push_back(temp);
}
