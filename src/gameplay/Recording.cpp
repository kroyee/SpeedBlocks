#include "Recording.h"
#include <fstream>
#include <iostream>
#include "GameSignals.h"
#include "NetworkPackets.hpp"
#include "PacketCompressReplay.h"

#ifdef __APPLE__
#include "ResourcePath.hpp"
#else
#include "EmptyResourcePath.h"
#endif

Recording::Recording() : name("") {
    connectSignal("GetRecDuration", [&]() -> const sf::Time& { return duration; });
    connectSignal("GetRecTime", [&]() -> sf::Time { return timer.getElapsedTime() + startAt; });
    connectSignal("RecJumpTo", &Recording::jumpTo, this);
    connectSignal("GetRecName", [&]() -> const std::string& { return name; });
    connectSignal("SendRecording", &Recording::sendRecording, this);
}

void Recording::clear() {
    for (int y = 0; y < 22; y++)
        for (int x = 0; x < 10; x++) starting_position[y][x] = 0;
    duration = sf::seconds(0);
    name = "";
    events.clear();
}

void Recording::start(std::array<std::array<uint8_t, 10>, 22> start_pos) {
    clear();
    for (int y = 0; y < 22; y++)
        for (int x = 0; x < 10; x++) starting_position[y][x] = start_pos[y][x];
    RecordingEvent event;
    event.type = 100;
    timer.restart();
    addEvent(event);
    rec = true;
}

void Recording::stop() {
    RecordingEvent event;
    event.type = 101;
    addEvent(event);
    duration = events.back().time;
    rec = false;
}

void Recording::jumpTo(int startTime) {
    timer.restart();
    startAt = sf::milliseconds(startTime);
    currentEvent = 0;
}

void Recording::addEvent(RecordingEvent& event) {
    event.time = timer.getElapsedTime();
    events.push_back(event);
}

void Recording::save(std::string filename) {
    if (filename == "") {
        time_t rawtime;
        time(&rawtime);
        filename = resourcePath() + "Recordings/" + ctime(&rawtime);
    }
    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open()) {
#ifdef DEBUG
        std::cout << "Error saving recording" << std::endl;
#endif

        return;
    }

    file.write((char*)starting_position, 220);

    uint32_t eventTime;
    for (auto&& event : events) {
        switch (event.type) {
            case 100:
                file.write((char*)&event.type, 1);
                eventTime = (uint32_t)event.time.asMilliseconds();
                file.write((char*)&eventTime, 4);
                break;
            case 101:
                file.write((char*)&event.type, 1);
                eventTime = (uint32_t)event.time.asMilliseconds();
                file.write((char*)&eventTime, 4);
                break;
            case 1:
                file.write((char*)&event.type, 1);

                file.write((char*)&event.piece, 1);
                file.write((char*)&event.rotation, 1);
                file.write((char*)&event.color, 1);
                file.write((char*)&event.x, 1);
                file.write((char*)&event.y, 1);

                eventTime = (uint32_t)event.time.asMilliseconds();
                file.write((char*)&eventTime, 4);

                file.write((char*)&event.pending, 1);
                file.write((char*)&event.combo, 1);
                file.write((char*)&event.comboTimer, 1);
                file.write((char*)&event.bpm, 1);
                break;
            case 2:
                file.write((char*)&event.type, 1);

                file.write((char*)&event.piece, 1);
                file.write((char*)&event.rotation, 1);
                file.write((char*)&event.color, 1);
                file.write((char*)&event.x, 1);
                file.write((char*)&event.y, 1);

                eventTime = (uint32_t)event.time.asMilliseconds();
                file.write((char*)&eventTime, 4);

                file.write((char*)&event.pending, 1);
                file.write((char*)&event.combo, 1);
                file.write((char*)&event.comboTimer, 1);
                file.write((char*)&event.bpm, 1);
                break;
            case 4:
                file.write((char*)&event.type, 1);

                file.write((char*)&event.x, 1);

                eventTime = (uint32_t)event.time.asMilliseconds();
                file.write((char*)&eventTime, 4);

                file.write((char*)&event.pending, 1);
                file.write((char*)&event.combo, 1);
                file.write((char*)&event.comboTimer, 1);
                file.write((char*)&event.bpm, 1);
                break;
            case 5:
                file.write((char*)&event.type, 1);

                eventTime = (uint32_t)event.time.asMilliseconds();
                file.write((char*)&eventTime, 4);

                file.write((char*)&event.pending, 1);
                file.write((char*)&event.combo, 1);
                file.write((char*)&event.comboTimer, 1);
                file.write((char*)&event.bpm, 1);
                break;
            case 6:
                file.write((char*)&event.type, 1);

                file.write((char*)&event.piece, 1);

                eventTime = (uint32_t)event.time.asMilliseconds();
                file.write((char*)&eventTime, 4);

                file.write((char*)&event.pending, 1);
                file.write((char*)&event.combo, 1);
                file.write((char*)&event.comboTimer, 1);
                file.write((char*)&event.bpm, 1);
                break;
            case 7:
                file.write((char*)&event.type, 1);

                file.write((char*)&event.pending, 1);

                eventTime = (uint32_t)event.time.asMilliseconds();
                file.write((char*)&eventTime, 4);

                file.write((char*)&event.pending, 1);
                file.write((char*)&event.combo, 1);
                file.write((char*)&event.comboTimer, 1);
                file.write((char*)&event.bpm, 1);
                break;
        }
    }
    file.close();
}

void Recording::load(std::string filename) {
    std::ifstream file(filename, std::ios::binary);

    if (!file.is_open()) {
#ifdef DEBUG
        std::cout << "Error loading recording" << std::endl;
#endif

        return;
    }

    events.clear();

    file.read((char*)starting_position, 220);

    RecordingEvent event;
    uint32_t eventTime;
    while (!file.eof()) {
        file.read((char*)&event.type, 1);
        switch (event.type) {
            case 100:
                file.read((char*)&eventTime, 4);
                break;
            case 101:
                file.read((char*)&eventTime, 4);
                break;
            case 1:
                file.read((char*)&event.piece, 1);
                file.read((char*)&event.rotation, 1);
                file.read((char*)&event.color, 1);
                file.read((char*)&event.x, 1);
                file.read((char*)&event.y, 1);

                file.read((char*)&eventTime, 4);

                file.read((char*)&event.pending, 1);
                file.read((char*)&event.combo, 1);
                file.read((char*)&event.comboTimer, 1);
                file.read((char*)&event.bpm, 1);
                break;
            case 2:
                file.read((char*)&event.piece, 1);
                file.read((char*)&event.rotation, 1);
                file.read((char*)&event.color, 1);
                file.read((char*)&event.x, 1);
                file.read((char*)&event.y, 1);

                file.read((char*)&eventTime, 4);

                file.read((char*)&event.pending, 1);
                file.read((char*)&event.combo, 1);
                file.read((char*)&event.comboTimer, 1);
                file.read((char*)&event.bpm, 1);
                break;
            case 4:
                file.read((char*)&event.x, 1);

                file.read((char*)&eventTime, 4);

                file.read((char*)&event.pending, 1);
                file.read((char*)&event.combo, 1);
                file.read((char*)&event.comboTimer, 1);
                file.read((char*)&event.bpm, 1);
                break;
            case 5:
                file.read((char*)&eventTime, 4);

                file.read((char*)&event.pending, 1);
                file.read((char*)&event.combo, 1);
                file.read((char*)&event.comboTimer, 1);
                file.read((char*)&event.bpm, 1);
                break;
            case 6:
                file.read((char*)&event.piece, 1);

                file.read((char*)&eventTime, 4);

                file.read((char*)&event.pending, 1);
                file.read((char*)&event.combo, 1);
                file.read((char*)&event.comboTimer, 1);
                file.read((char*)&event.bpm, 1);
                break;
            case 7:
                file.read((char*)&event.pending, 1);

                file.read((char*)&eventTime, 4);

                file.read((char*)&event.pending, 1);
                file.read((char*)&event.combo, 1);
                file.read((char*)&event.comboTimer, 1);
                file.read((char*)&event.bpm, 1);
                break;
        }
        event.time = sf::milliseconds(eventTime);
        events.push_back(event);
    }
    duration = events.back().time;
    file.close();
}

void Recording::sendRecording(int type) {
    NP_Replay replay;
    replay.type = type;
    replay.name = name;
    replay.id = id;
    PacketCompressReplay compressor;
    compressor.compressReplay(*this, replay.data);
    PM::write(replay);
}

void Recording::receiveRecording(const NP_Replay& p) {
    name = p.name;
    id = p.id;
    events.clear();
    PacketCompressReplay compressor;
    compressor.extractReplay(*this, p);
}
