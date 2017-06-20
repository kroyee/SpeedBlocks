#ifndef RECORDING_H
#define RECORDING_H

#include <SFML/Network.hpp>

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

class network;

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
	sf::Uint8 starting_position[22][10];
	std::vector<RecordingEvent> events;
	sf::Time duration, startAt;
	sf::String name;
	sf::Clock timer;
	sf::Uint32 currentEvent;

	sf::Uint8 prevCombo;
	bool rec, halt;

	void clear();
	void start(sf::Uint8[22][10]);
	void stop();
	void jumpTo(sf::Uint32 startTime);
	void addEvent(RecordingEvent& event);
	void save(std::string filename="");
	void load(std::string filename);

	void sendRecording(network& net, sf::Uint16 type);
	void receiveRecording(network& net);
};

#endif