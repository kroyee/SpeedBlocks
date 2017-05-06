#include "Recording.h"

#ifdef __APPLE__
#include "ResourcePath.hpp"
#else
#include "EmptyResourcePath.h"
#endif

void Recording::clear() {
	for (int y=0; y<22; y++)
		for (int x=0; x<10; x++)
			starting_position[y][x] = 0;
	duration = sf::seconds(0);
	name = "";
	while (events.size() > 0)
		events.pop_back();
}

void Recording::start() {
	clear();
	RecordingEvent event;
	event.type=100;
	timer.restart();
	addEvent(event);
	rec=true;
}

void Recording::stop() {
	RecordingEvent event;
	event.type=101;
	addEvent(event);
	duration = events.back().time;
	rec=false;
}

void Recording::addEvent(RecordingEvent& event) {
	event.time = timer.getElapsedTime();
	events.push_back(event);
}

void Recording::save() {
	ofstream file(resourcePath() + "Recordings/" + name);
	int duplicate_counter=2;
	while (!file.is_open()) {
		if (duplicate_counter == 1000)
			return;
		ofstream file(resourcePath() + "Recordings/" + name + "-" + to_string(duplicate_counter));
	}

	for (auto&& event : events) {
		switch(event.type) {
			case 100:
				file << "100.";
			break;
			case 101:
				file << "101.";
			break;
			case 1:
				file << "1." << (int)event.piece << "." << (int)event.rotation << "." << (int)event.color << ".";
				file << (int)event.x << "." << (int)event.y << "." << time.asMilliseconds() << ".";
			break;
			case 2:
				file << "2." << (int)event.piece << "." << (int)event.rotation << "." << (int)event.color << ".";
				file << (int)event.x << "." << (int)event.y << "." << time.asMilliseconds() << ".";
			break;
			case 3:
				file << "3." << (int)event.y << "." << time.asMilliseconds();
			break;
		}
	}
}