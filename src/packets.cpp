#include "gui.h"
#include "network.h"
#include "gamePlay.h"

void UI::sendPacket0(sf::Uint16 id) {
	sf::Uint8 packetid = 0;
	net.packet.clear();
	net.packet << packetid << id;
	net.sendTCP();
}

void UI::sendPacket1() {
	sf::Uint8 packetid = 1;
	net.packet.clear();
	net.packet << packetid;
	net.sendTCP();
}

void UI::sendPacket2(const sf::String& name, const sf::String& pass, sf::Uint8 guest) {
	sf::Uint8 packetid = 2;
	net.packet.clear();
	net.packet << packetid << clientVersion << guest << name << pass;
	net.sendTCP();
}

void UI::sendPacket3() {
	sf::Uint8 packetid = 3;
	net.packet.clear();
	net.packet << packetid << game.maxCombo << game.linesSent << game.linesRecieved << game.linesBlocked << game.bpm << game.linesPerMinute;
	net.sendTCP();
}

void UI::sendPacket4() {
	sf::Uint8 packetid = 4;
	net.packet.clear();
	net.packet << packetid << game.maxCombo << game.linesSent << game.linesRecieved << game.linesBlocked << game.bpm << game.linesPerMinute;
	net.sendTCP();
}

void UI::sendPacket5(sf::Uint8 amount) {
	sf::Uint8 packetid=5;
	net.packet.clear();
	net.packet << packetid << amount;
	net.sendTCP();
}

void UI::sendPacket6(sf::Uint8 amount) {
	sf::Uint8 packetid=6;
	net.packet.clear();
	net.packet << packetid << amount;
	net.sendTCP();
}

void UI::sendPacket7(sf::Uint8 amount) {
	sf::Uint8 packetid=7;
	net.packet.clear();
	net.packet << packetid << amount;
	net.sendTCP();
}

void UI::sendPacket8() {
	sf::Uint8 packetid = 8;
	net.packet.clear();
	net.packet << packetid;
	net.sendTCP();
}

void UI::sendPacket9() {
	sf::Uint8 packetid = 9;
	net.packet.clear();
	net.packet << packetid;
	net.sendTCP();
}

void UI::sendPacket10(const sf::String& to, const sf::String& msg) {
	sf::Uint8 packetid = 10;
	net.packet.clear();
	net.packet << packetid;
	if (to == "Room") {
		packetid = 1;
		net.packet << packetid << msg;
		net.sendTCP();
	}
	else if (to == "Lobby") {
		packetid = 2;
		net.packet << packetid << msg;
		net.sendTCP();
	}
	else {
		packetid = 3;
		net.packet << packetid << to << msg;
		net.sendTCP();
	}
}

void UI::sendPacket11(const sf::String& name, sf::Uint8 maxPlayers) {
	sf::Uint8 packetid = 11;
	net.packet.clear();
	net.packet << packetid << name << maxPlayers;
	net.sendTCP();
}

void UI::sendPacket12() {
	sf::Uint8 packetid = 12;
	net.packet.clear();
	net.packet << packetid;
	net.sendTCP();
}

void UI::sendPacket13() {
	sf::Uint8 packetid = 13;
	net.packet.clear();
	net.packet << packetid;
	net.sendTCP();
}

//Sign up for tournament
void UI::sendPacket14(sf::Uint16 id) {
	sf::Uint8 packetid = 14;
	net.packet.clear();
	net.packet << packetid << id;
	net.sendTCP();
}

//Withdraw from tournament
void UI::sendPacket15(sf::Uint16 id) {
	sf::Uint8 packetid = 15;
	net.packet.clear();
	net.packet << packetid << id;
	net.sendTCP();
}

//Close signup of tournament
void UI::sendPacket16(sf::Uint16 id) {
	sf::Uint8 packetid = 16;
	net.packet.clear();
	net.packet << packetid << id;
	net.sendTCP();
}

//Start tournament
void UI::sendPacket17(sf::Uint16 id) {
	sf::Uint8 packetid = 17;
	net.packet.clear();
	net.packet << packetid << id;
	net.sendTCP();
}

//Join tournament game
void UI::sendPacket18(sf::Uint16 tournamentId, sf::Uint16 gameId) {
	sf::Uint8 packetid = 18;
	net.packet.clear();
	net.packet << packetid << tournamentId << gameId;
	net.sendTCP();
}

//Tournament-panel close (no longer need updated from torunament)
void UI::sendPacket19(sf::Uint16 tournamentId) {
	sf::Uint8 packetid = 19;
	net.packet.clear();
	net.packet << packetid << tournamentId;
	net.sendTCP();
}

void UI::sendPacket99() {
	sf::Uint8 packetid = 99;
	net.packet.clear();
	net.packet << packetid << myId;
	net.sendUDP();
}

void UI::sendPacket100() {
	if (gamestate == CountDown) {
		sf::Uint8 tmp = game.field.piece.piece;
		game.field.piece.piece = 7; // makes the current piece not draw on other players screen (since it's countdown)
		compressor.compress();
		game.field.piece.piece = tmp;
	}
	else
		compressor.compress();
	net.packet.clear();
	sf::Uint8 packetid = 100;
	net.packet << packetid << myId << gamedatacount;
	gamedatacount++;
	for (int i=0; i<compressor.tmpcount; i++)
		net.packet << compressor.tmp[i];
	if (compressor.bitcount>0)
		net.packet << compressor.tmp[compressor.tmpcount];
	net.sendUDP();
}

// Ping packet to check latency with server
void UI::sendPacket102(sf::Uint8 pingId) {
	net.packet.clear();
	sf::Uint8 packetid = 102;
	net.packet << packetid << myId << pingId;
	net.sendUDP();
}