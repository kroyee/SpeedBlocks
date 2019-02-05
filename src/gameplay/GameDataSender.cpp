#include "GameDataSender.h"
#include "GamePlay.h"
#include "GameSignals.h"
#include "PacketCompressReplay.h"
#include "Packets.hpp"
#include "Resources.h"

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

    PacketCompressReplay compressor;

    if (game.resources.gamestate == GameStates::CountDown) {
        uint8_t tmp = game.field.piece.piece;
        game.field.piece.piece = 7;  // makes the current piece not draw on other players screen (since it's countdown)
        compressor = game;
        compressor.compress();
        game.field.piece.piece = tmp;
    } else
        compressor.compress();

    UDP.write(NP_Gamestate{game.resources.myId, count++, compressor.m_data});
}

void GameDataSender::gameover() {
    if (!game.resources.playonline || game.resources.gamestate == GameStates::Practice) return;

    TCP.write(NP_GameOver{{static_cast<uint8_t>(game.data.maxCombo), game.data.linesSent, game.data.linesRecieved, game.data.linesBlocked, game.data.bpm},
                          static_cast<uint32_t>(game.recorder.duration.asMilliseconds()),
                          game.data.pieceCount});

    state();
}

void GameDataSender::reset() {
    count = 251;
    sendTime = sf::seconds(0);
}