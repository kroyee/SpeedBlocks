#include "GameDataSender.h"
#include "GamePlay.h"
#include "GameSignals.h"
#include "NetworkPackets.hpp"
#include "Resources.h"
#include "packetcompress.h"

static auto& Survivor = Signal<bool>::get("Survivor");

GameDataSender::GameDataSender(GamePlay& _game) : game(_game), sendTime(sf::seconds(0)), count(251) { connectSignal("SendGameState", &GameDataSender::state, this); }

void GameDataSender::sendstate() {
    if (game.resources.gamestate == GameStates::Practice) return;

    sf::Time tmp = game.gameclock.getElapsedTime();
    if (tmp < sendTime) return;

    sendTime += sf::milliseconds(100);
    if (sendTime < tmp) sendTime = tmp + sf::milliseconds(100);

    state();
}

void GameDataSender::state() {
    if (!game.resources.playonline) return;

    if (game.resources.gamestate == GameStates::CountDown) {
        uint8_t tmp = game.field.piece.piece;
        game.field.piece.piece = 7;  // makes the current piece not draw on other players screen (since it's countdown)
        game.resources.compressor->compress();
        game.field.piece.piece = tmp;
    } else
        game.resources.compressor->compress();

    PM::write_udp(NP_Gamestate{game.resources.myId, count++, game.resources.compressor->tmp});
}

void GameDataSender::gameover() {
    if (!game.resources.playonline || game.resources.gamestate == GameStates::Practice) return;

    PM::write(NP_GameOver{{static_cast<uint8_t>(game.data.maxCombo), game.data.linesSent, game.data.linesRecieved, game.data.linesBlocked, game.data.bpm},
                          static_cast<uint32_t>(game.recorder.duration.asMilliseconds()),
                          game.data.pieceCount});

    state();
}

void GameDataSender::reset() {
    count = 251;
    sendTime = sf::seconds(0);
}