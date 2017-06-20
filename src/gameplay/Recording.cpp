#include "Recording.h"
#include "network.h"
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

void Recording::start(sf::Uint8 start_pos[22][10]) {
	clear();
	for (int y=0; y<22; y++)
		for (int x=0; x<10; x++)
			starting_position[y][x] = start_pos[y][x];
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

void Recording::save(std::string filename) {
	if (filename == "") {
		time_t rawtime;
		time(&rawtime);
		filename = resourcePath() + "Recordings/" + ctime(&rawtime);
	}
	std::ofstream file(filename, std::ios::binary);
	if (!file.is_open()) {
		std::cout << "Error saving recording" << std::endl;
		return;
	}

	file.write((char*)starting_position, 220);

	sf::Uint32 eventTime;
	for (auto&& event : events) {
		switch(event.type) {
			case 100:
				file.write((char*)&event.type, 1);
				eventTime = (sf::Uint32)event.time.asMilliseconds();
				file.write((char*)&eventTime, 4);
			break;
			case 101:
				file.write((char*)&event.type, 1);
				eventTime = (sf::Uint32)event.time.asMilliseconds();
				file.write((char*)&eventTime, 4);
			break;
			case 1:
				file.write((char*)&event.type, 1);

				file.write((char*)&event.piece, 1);
				file.write((char*)&event.rotation, 1);
				file.write((char*)&event.color, 1);
				file.write((char*)&event.x, 1);
				file.write((char*)&event.y, 1);

				eventTime = (sf::Uint32)event.time.asMilliseconds();
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

				eventTime = (sf::Uint32)event.time.asMilliseconds();
				file.write((char*)&eventTime, 4);

				file.write((char*)&event.pending, 1);
				file.write((char*)&event.combo, 1);
				file.write((char*)&event.comboTimer, 1);
				file.write((char*)&event.bpm, 1);
			break;
			case 4:
				file.write((char*)&event.type, 1);

				file.write((char*)&event.x, 1);

				eventTime = (sf::Uint32)event.time.asMilliseconds();
				file.write((char*)&eventTime, 4);

				file.write((char*)&event.pending, 1);
				file.write((char*)&event.combo, 1);
				file.write((char*)&event.comboTimer, 1);
				file.write((char*)&event.bpm, 1);
			break;
			case 5:
				file.write((char*)&event.type, 1);

				eventTime = (sf::Uint32)event.time.asMilliseconds();
				file.write((char*)&eventTime, 4);

				file.write((char*)&event.pending, 1);
				file.write((char*)&event.combo, 1);
				file.write((char*)&event.comboTimer, 1);
				file.write((char*)&event.bpm, 1);
			break;
			case 6:
				file.write((char*)&event.type, 1);

				file.write((char*)&event.piece, 1);

				eventTime = (sf::Uint32)event.time.asMilliseconds();
				file.write((char*)&eventTime, 4);

				file.write((char*)&event.pending, 1);
				file.write((char*)&event.combo, 1);
				file.write((char*)&event.comboTimer, 1);
				file.write((char*)&event.bpm, 1);
			break;
			case 7:
				file.write((char*)&event.type, 1);

				file.write((char*)&event.pending, 1);

				eventTime = (sf::Uint32)event.time.asMilliseconds();
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
		std::cout << "Error loading recording" << std::endl;
		return;
	}

	events.clear();

	file.read((char*)starting_position, 220);

	RecordingEvent event;
	sf::Uint32 eventTime;
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

void Recording::sendRecording(network& net, sf::Uint16 type) {
	net.packet.clear();
	net.packet << (sf::Uint8)1 << type;
	for (int y=0; y<22; y++)
		for (int x=0; x<10; x++)
			net.packet << starting_position[y][x];
	for (auto&& event : events) {
		switch(event.type) {
			case 100:
				net.packet << event.type << (sf::Uint32)event.time.asMilliseconds();
			break;
			case 101:
				net.packet << event.type << (sf::Uint32)event.time.asMilliseconds();
			break;
			case 1:
				net.packet << event.type << event.piece << event.rotation << event.color;
				net.packet << event.x << event.y << (sf::Uint32)event.time.asMilliseconds();
				net.packet << event.pending << event.combo << event.comboTimer << event.bpm;
			break;
			case 2:
				net.packet << event.type << event.piece << event.rotation << event.color;
				net.packet << event.x << event.y << (sf::Uint32)event.time.asMilliseconds();
				net.packet << event.pending << event.combo << event.comboTimer << event.bpm;
			break;
			case 4:
				net.packet << event.type << event.x << (sf::Uint32)event.time.asMilliseconds();
				net.packet << event.pending << event.combo << event.comboTimer << event.bpm;
			break;
			case 5:
				net.packet << event.type << (sf::Uint32)event.time.asMilliseconds();
				net.packet << event.pending << event.combo << event.comboTimer << event.bpm;
			break;
			case 6:
				net.packet << event.type << event.piece << (sf::Uint32)event.time.asMilliseconds();
				net.packet << event.pending << event.combo << event.comboTimer << event.bpm;
			break;
			case 7:
				net.packet << event.type << event.pending << (sf::Uint32)event.time.asMilliseconds();
				net.packet << event.pending << event.combo << event.comboTimer << event.bpm;
			break;
		}
	}
	net.sendTCP();
}

void Recording::receiveRecording(network& net) {
	events.clear();

	for (int y=0; y<22; y++)
		for (int x=0; x<10; x++)
			net.packet >> starting_position[y][x];

	RecordingEvent event;
	sf::Uint32 eventTime;
	while (true) {
		net.packet >> event.type;
		switch (event.type) {
			case 100:
				net.packet >> eventTime;
			break;
			case 101:
				net.packet >> eventTime;
			break;
			case 1:
				net.packet >> event.piece >> event.rotation >> event.color;
				net.packet >> event.x >> event.y >> eventTime;
				net.packet >> event.pending >> event.combo >> event.comboTimer >> event.bpm;
			break;
			case 2:
				net.packet >> event.piece >> event.rotation >> event.color;
				net.packet >> event.x >> event.y >> eventTime;
				net.packet >> event.pending >> event.combo >> event.comboTimer >> event.bpm;
			break;
			case 4:
				net.packet >> event.x >> eventTime;
				net.packet >> event.pending >> event.combo >> event.comboTimer >> event.bpm;
			break;
			case 5:
				net.packet >> eventTime;
				net.packet >> event.pending >> event.combo >> event.comboTimer >> event.bpm;
			break;
			case 6:
				net.packet >> event.piece >> eventTime;
				net.packet >> event.pending >> event.combo >> event.comboTimer >> event.bpm;
			break;
			case 7:
				net.packet >> event.pending >> eventTime;
				net.packet >> event.pending >> event.combo >> event.comboTimer >> event.bpm;
			break;
			default:
			break;
		}
		event.time = sf::milliseconds(eventTime);
		events.push_back(event);
		if (event.type == 101) {
			duration=event.time;
			return;
		}
	}
}