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

class RecordingEvent {
public:
	sf::Uint8 type;
	sf::Uint8 piece, rotation, color;
	sf::Uint8 pending, combo, comboTimer, bpm;
	sf::Int8 x, y;
	sf::Time time;
};

class Recording {
public:
	Recording();
	sf::Uint8 starting_position[22][10];
	std::vector<RecordingEvent> events;
	sf::Time duration, startAt, comboSet;
	sf::String name;
	sf::Clock timer;
	sf::Uint32 currentEvent;

	sf::Uint8 prevCombo, comboTimer, lastComboTimer;
	bool rec, halt;

	void clear();
	void start(std::array<std::array<sf::Uint8, 10>, 22>);
	void stop();
	void jumpTo(int startTime);
	void addEvent(RecordingEvent& event);
	void save(std::string filename="");
	void load(std::string filename);

	void sendRecording(int type);
	void receiveRecording(sf::Packet &packet);
};

#endif