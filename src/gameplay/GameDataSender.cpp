#include "GameDataSender.h"
#include "GamePlay.h"
#include "Resources.h"
#include "GameSignals.h"
#include "packetcompress.h"

static auto& SendPacketUDP = Signal<void, sf::Packet&>::get("SendPacketUDP");
static auto& SendPacket = Signal<void, sf::Packet&>::get("SendPacket");
static auto& Survivor = Signal<bool>::get("Survivor");

GameDataSender::GameDataSender(GamePlay& _game) : game(_game), sendTime(sf::seconds(0)), count(251) {
	connectSignal("SendGameState", &GameDataSender::state, this);
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
		uint8_t tmp = game.field.piece.piece;
		game.field.piece.piece = 7; // makes the current piece not draw on other players screen (since it's countdown)
		game.resources.compressor->compress();
		game.field.piece.piece = tmp;
	}
	else
		game.resources.compressor->compress();
	sf::Packet packet;
	packet << (uint8_t)100 << game.resources.myId << count;
	count++;
	for (auto i : game.resources.compressor->tmp)
		packet << i;
	SendPacketUDP(packet);
}

void GameDataSender::gameover(int winner) {
	if (!game.resources.playonline || game.resources.gamestate == GameStates::Practice)
		return;

	sf::Packet packet;
	if (winner || Survivor()) {
		packet << (uint8_t)4 << (uint8_t)game.combo.maxCombo << (uint16_t)game.data.linesSent << (uint16_t)game.data.linesRecieved << (uint16_t)game.garbage.linesBlocked;
		packet << (uint16_t)game.data.bpm << (uint32_t)game.recorder.duration.asMilliseconds();
		packet << (uint16_t)game.data.pieceCount;
	}
	else
		packet << (uint8_t)3 << (uint8_t)game.combo.maxCombo << (uint16_t)game.data.linesSent << (uint16_t)game.data.linesRecieved << (uint16_t)game.garbage.linesBlocked << (uint16_t)game.data.bpm;
	
	SendPacket(packet);
	state();
}

void GameDataSender::reset() {
	count = 251;
	sendTime = sf::seconds(0);
}