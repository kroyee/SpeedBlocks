#ifndef RECORDING_H
#define RECORDING_H

#include <SFML/Network.hpp>
#include <array>

// List of types for RecordingEvent:
// 1 : Piece moved/rotated
// 2 : Piece added to playfield
// 3 : Not used
// 4 : Garbage was added, x holds the hole
// 5 : Update pending/bpm/combo - All other events will do this aswell, but this is for updating when nothing else happens
// 6 : Update nextpiece
// 7 : Countdown, pending holds the current count
// 100 : Start of recording
// 101 : End of recording

struct NP_Replay;

class RecordingEvent {
   public:
    uint8_t type;
    uint8_t piece, rotation, color;
    uint8_t pending, combo, comboTimer, bpm;
    int8_t x, y;
    sf::Time time;
};

class Recording {
   public:
    Recording();
    uint8_t starting_position[22][10];
    std::vector<RecordingEvent> events;
    sf::Time duration, startAt, comboSet;
    std::string name;
    uint16_t id;
    sf::Clock timer;
    uint32_t currentEvent;

    uint8_t prevCombo, comboTimer, lastComboTimer;
    bool rec, halt;

    void clear();
    void start(std::array<std::array<uint8_t, 10>, 22>);
    void stop();
    void jumpTo(int startTime);
    void addEvent(RecordingEvent& event);
    void save(std::string filename = "");
    void load(std::string filename);

    void sendRecording(int type);
    void receiveRecording(const NP_Replay&);
};

#endif