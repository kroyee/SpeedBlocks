#ifndef RECORDING_H
#define RECORDING_H

#include <SFML/Graphics.hpp>

// List of types for RecordingEvent:
// 1 : Piece moved/rotated
// 2 : Piece added to playfield
// 3 : Line was cleared, y holds the line to be cleared
// 100 : Start of recording
// 101 : End of recording

class RecordingEvent {
public:
	sf::Uint8 type;
	sf::Uint8 piece, rotation, color;
	sf::Int8 x, y;
	sf::Time time;
};

class Recording {
public:
	sf::Uint8 starting_position[22][10];
	std::vector<RecordingEvent> events;
	sf::Time duration;
	sf::String name;
	sf::Clock timer;

	bool rec;

	void clear();
	void start();
	void stop();
	void addEvent(RecordingEvent& event);
	void save();
};

#endif