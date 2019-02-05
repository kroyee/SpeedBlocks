#include "PacketCompressReplay.h"
#include <SFML/System.hpp>
#include <algorithm>
#include "GameField.h"
#include "GamePlay.h"
#include "Packets.hpp"
#include "Recording.h"

void PacketCompressReplay::compressReplay(Recording& replay, std::vector<uint8_t>& data) {
    m_data.clear();
    tmpcount = 0;
    bitcount = 0;
    uint8_t counter = 0;
    int y, endy;
    for (endy = 21; endy >= 0; endy--) {
        if (replay.starting_position[endy][0] == 8 || replay.starting_position[endy][1] == 8)
            counter++;
        else
            break;
    }
    addBits(counter, 5);
    for (y = 21; y > endy; y--)
        for (uint8_t x = 0; x < 10; x++)
            if (replay.starting_position[y][x] == 0) {
                addBits(x, 4);
                break;
            }
    for (int x = 0; x < 10; x++) {
        counter = 0;
        for (y = 0; y <= endy; y++) {
            if (!replay.starting_position[y][x])
                counter++;
            else
                break;
        }
        addBits(counter, 5);
        for (; y <= endy; y++) {
            addBits(replay.starting_position[y][x], 3);
        }
    }

    for (auto&& event : replay.events) {
        switch (event.type) {
            case 100:
                addBits(0, 3);
                addTimeStamp(event.time);
                break;
            case 101:
                addBits(3, 3);
                addTimeStamp(event.time);
                break;
            case 1:
                addBits(event.type, 3);
                addBits(event.piece, 3);
                addBits(event.rotation, 2);
                addBits(event.color, 3);
                addBits(event.x + 2, 4);
                addBits(event.y, 5);
                addTimeStamp(event.time);
                break;
            case 2:
                addBits(event.type, 3);
                addBits(event.piece, 3);
                addBits(event.rotation, 2);
                addBits(event.color, 3);
                addBits(event.x + 2, 4);
                addBits(event.y, 5);
                addTimeStamp(event.time);
                break;
            case 4:
                addBits(event.type, 3);
                addBits(event.x, 4);
                addTimeStamp(event.time);
                break;
            case 5:
                addBits(event.type, 3);
                addTimeStamp(event.time);
                addBits(event.pending, 8);
                addBits(event.combo, 5);
                addBits(event.comboTimer, 7);
                break;
            case 6:
                addBits(event.type, 3);
                addBits(event.piece, 3);
                addTimeStamp(event.time);
                break;
            case 7:
                addBits(event.type, 3);
                addBits(event.pending, 2);
                addTimeStamp(event.time);
                break;
        }
    }

    data = std::move(m_data);
}

void PacketCompressReplay::addTimeStamp(sf::Time& stamp) {
    uint16_t timevalue = stamp.asMilliseconds() % 2048;
    addBigBits(timevalue, 11);
}

void PacketCompressReplay::getTimeStamp(sf::Time& stamp) {
    uint32_t timevalue;
    getBigBits(timevalue, 11);
    while (timevalue < lastTime) timevalue += 2048;
    lastTime = timevalue;
    stamp = sf::milliseconds(timevalue);
}

void PacketCompressReplay::extractReplay(Recording& replay, const NP_Replay& p) {
    replay.clear();
    m_data.clear();
    tmpcount = 100;
    bitcount = 0;

    loadTmp(p.data);

    uint8_t counter = 0;
    uint8_t endy = 0;
    uint8_t temp = 0;
    int y;
    getBits(endy, 5);
    for (int c = 0; c < endy; c++) {
        for (int x = 0; x < 10; x++) replay.starting_position[21 - c][x] = 8;
        getBits(temp, 4);
        replay.starting_position[21 - c][temp] = 0;
    }
    for (int x = 0; x < 10; x++) {
        counter = 0;
        getBits(counter, 5);
        for (y = 0; y < counter; y++) replay.starting_position[y][x] = 0;
        for (; y < 22 - endy; y++) getBits(replay.starting_position[y][x], 3);
    }

    RecordingEvent event;
    lastTime = 0;
    while (true) {
        getBits(event.type, 3);
        switch (event.type) {
            case 0:
                event.type = 100;
                getTimeStamp(event.time);
                break;
            case 3:
                event.type = 101;
                getTimeStamp(event.time);
                break;
            case 1:
                getBits(event.piece, 3);
                getBits(event.rotation, 2);
                getBits(event.color, 3);
                getBits(temp, 4);
                event.x = temp - 2;
                getBits(temp, 5);
                event.y = temp;
                getTimeStamp(event.time);
                break;
            case 2:
                getBits(event.piece, 3);
                getBits(event.rotation, 2);
                getBits(event.color, 3);
                getBits(temp, 4);
                event.x = temp - 2;
                getBits(temp, 5);
                event.y = temp;
                getTimeStamp(event.time);
                break;
            case 4:
                getBits(temp, 4);
                event.x = temp;
                getTimeStamp(event.time);
                break;
            case 5:
                getTimeStamp(event.time);
                getBits(event.pending, 8);
                getBits(event.combo, 5);
                getBits(event.comboTimer, 7);
                break;
            case 6:
                getBits(event.piece, 3);
                getTimeStamp(event.time);
                break;
            case 7:
                getBits(event.pending, 2);
                getTimeStamp(event.time);
                break;
            default:
                break;
        }
        replay.events.push_back(event);
        if (event.type == 101) {
            replay.duration = event.time;
            return;
        }
    }
}

PacketCompressReplay& PacketCompressReplay::operator=(const GamePlay& game) {
    square = game.field.square;
    posX = game.field.piece.posX + 2;
    posY = game.field.piece.posY;
    piece = game.field.piece.piece;
    color = game.field.piece.tile;
    rotation = game.field.piece.current_rotation;
    nextpiece = game.nextpiece;
    if (nextpiece < 7) {
        npcol = game.basepiece[nextpiece].tile;
        nprot = Options::get_piece_rotation(nextpiece);
    } else {
        npcol = nprot = 0;
    }
    comboText = game.field.text.get<FieldText::Combo>();
    pendingText = game.field.text.get<FieldText::Pending>();
    bpmText = std::min(game.field.text.get<FieldText::BPM>(), 255);
    comboTimerCount = game.field.text.comboTimer.getPointCount() - 2;
    countdown = game.field.text.get<FieldText::Countdown>();
    time_val = game.gameclock.getElapsedTime().asMilliseconds();
    return *this;
}