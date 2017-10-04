#include "GameDataSender.h"
#include "gamePlay.h"
#include "Resources.h"
#include "GameSignals.h"
#include "packetcompress.h"

GameDataSender::GameDataSender(gamePlay& _game) : game(_game), sendTime(sf::seconds(0)), count(251) {
	Signals::SendGameState.connect(&GameDataSender::state, this);
}

void GameDataSender::sendstate() {
	if (game.resources.gamestate == GameStates::Practice)
		return;

	sf::Time tmp = game.gameclock.getElapsedTime();
	if (tmp<sendTime)
		return;

	sendTime+=sf::milliseconds(100);
	if (sendTime < tmp)
		sendTime=tmp+sf::milliseconds(100);

	state();
}

void GameDataSender::state() {
	if (!game.resources.playonline)
		return;
	
	if (game.resources.gamestate == GameStates::CountDown) {
		sf::Uint8 tmp = game.field.piece.piece;
		game.field.piece.piece = 7; // makes the current piece not draw on other players screen (since it's countdown)
		game.resources.compressor->compress();
		game.field.piece.piece = tmp;
	}
	else
		game.resources.compressor->compress();
	sf::Packet packet;
	packet << (sf::Uint8)100 << game.resources.myId << count;
	count++;
	for (int i=0; i<game.resources.compressor->tmpcount; i++)
		packet << game.resources.compressor->tmp[i];
	if (game.resources.compressor->bitcount>0)
		packet << game.resources.compressor->tmp[game.resources.compressor->tmpcount];
	Signals::SendPacketUDP(packet);
}

void GameDataSender::gameover(int winner) {
	if (!game.resources.playonline || game.resources.gamestate == GameStates::Practice)
		return;

	sf::Packet packet;
	if (winner || Signals::Survivor()) {
		packet << (sf::Uint8)4 << (sf::Uint8)game.combo.maxCombo << (sf::Uint16)game.data.linesSent << (sf::Uint16)game.data.linesRecieved << (sf::Uint16)game.garbage.linesBlocked;
		packet << (sf::Uint16)game.data.bpm << (sf::Uint32)game.recorder.duration.asMilliseconds();
		packet << (sf::Uint16)game.data.pieceCount;
	}
	else
		packet << (sf::Uint8)3 << (sf::Uint8)game.combo.maxCombo << (sf::Uint16)game.data.linesSent << (sf::Uint16)game.data.linesRecieved << (sf::Uint16)game.garbage.linesBlocked << (sf::Uint16)game.data.bpm;
	
	Signals::SendPacket(packet);
	state();
}

void GameDataSender::reset() {
	count = 251;
	sendTime = sf::seconds(0);
}