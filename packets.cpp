#include "gui.h"
#include "network.h"
#include "gamePlay.h"

void UI::sendPacket0(sf::Uint16 id) {
	sf::Uint8 packetid = 0;
	net->packet.clear();
	net->packet << packetid << id;
	net->sendTCP();
}

void UI::sendPacket1() {
	sf::Uint8 packetid = 1;
	net->packet.clear();
	net->packet << packetid;
	net->sendTCP();
}

void UI::sendPacket2(const sf::String& name, const sf::String& pass, sf::Uint8 guest) {
	sf::Uint8 packetid = 2;
	net->packet.clear();
	net->packet << packetid << clientVersion << guest << name << pass;
	net->sendTCP();
}

void UI::sendPacket3() {
	sf::Uint8 packetid = 3;
	net->packet.clear();
	net->packet << packetid << game->maxCombo << game->linesSent << game->linesRecieved << game->linesBlocked << game->bpm << game->linesPerMinute;
	net->sendTCP();
}

void UI::sendPacket4() {
	sf::Uint8 packetid = 4;
	net->packet.clear();
	net->packet << packetid << game->maxCombo << game->linesSent << game->linesRecieved << game->linesBlocked << game->bpm << game->linesPerMinute;
	net->sendTCP();
}

void UI::sendPacket5(sf::Uint8 amount) {
	sf::Uint8 packetid=5;
	net->packet.clear();
	net->packet << packetid << amount;
	net->sendTCP();
}

void UI::sendPacket6(sf::Uint8 amount) {
	sf::Uint8 packetid=6;
	net->packet.clear();
	net->packet << packetid << amount;
	net->sendTCP();
}

void UI::sendPacket7(sf::Uint8 amount) {
	sf::Uint8 packetid=7;
	net->packet.clear();
	net->packet << packetid << amount;
	net->sendTCP();
}

void UI::sendPacket8() {
	sf::Uint8 packetid = 8;
	net->packet.clear();
	net->packet << packetid;
	net->sendTCP();
}

void UI::sendPacket9() {
	sf::Uint8 packetid = 9;
	net->packet.clear();
	net->packet << packetid;
	net->sendTCP();
}

void UI::sendPacket10(const sf::String& to, const sf::String& msg) {
	sf::Uint8 packetid = 10;
	net->packet.clear();
	net->packet << packetid;
	if (to == "Room") {
		packetid = 1;
		net->packet << packetid << msg;
		net->sendTCP();
	}
	else if (to == "Lobby") {
		packetid = 2;
		net->packet << packetid << msg;
		net->sendTCP();
	}
	else {
		packetid = 3;
		net->packet << packetid << to << msg;
		net->sendTCP();
	}
}

void UI::sendPacket11(const sf::String& name, sf::Uint8 maxPlayers) {
	sf::Uint8 packetid = 11;
	net->packet.clear();
	net->packet << packetid << name << maxPlayers;
	net->sendTCP();
}

void UI::sendPacket101() {
	sf::Uint8 packetid = 101;
	net->packet.clear();
	net->packet << packetid << myId;
	net->sendUDP();
}