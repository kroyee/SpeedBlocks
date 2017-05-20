#include "Recording.h"
#include <iostream>
#include <fstream>

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
	events.clear();
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

void Recording::jumpTo(sf::Uint32 startTime) {
	timer.restart();
	startAt = sf::milliseconds(startTime);
	currentEvent=0;
}

void Recording::addEvent(RecordingEvent& event) {
	event.time = timer.getElapsedTime();
	events.push_back(event);
}

void Recording::save() {
	time_t rawtime;
	time(&rawtime);
	std::ofstream file(resourcePath() + "Recordings/" + name + " " + ctime(&rawtime), std::ios::trunc);
	while (!file.is_open()) {
		std::cout << "Error saving recording" << std::endl;
		return;
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
				file << (int)event.x << "." << (int)event.y << "." << event.time.asMilliseconds() << ".";
			break;
			case 2:
				file << "2." << (int)event.piece << "." << (int)event.rotation << "." << (int)event.color << ".";
				file << (int)event.x << "." << (int)event.y << "." << event.time.asMilliseconds() << ".";
			break;
			case 3:
				file << "3." << (int)event.y << "." << event.time.asMilliseconds();
			break;
		}
	}
}