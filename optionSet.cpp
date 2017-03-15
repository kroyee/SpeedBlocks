#include "optionSet.h"
#include <iostream>
#include <fstream>
#include <string>
#include <SFML/Graphics.hpp>
using namespace std;

short optionSet::loadOptions() {
	string line;
	ifstream file ("options.cfg");

	int countset = 0;

	bool success = 1;

	if (file.is_open()) {
		while (getline(file, line)) {
			switch (countset) {
				case 0: down = sf::Keyboard::Key(stoi(line)); break;
				case 1: left = sf::Keyboard::Key(stoi(line)); break;
				case 2: right = sf::Keyboard::Key(stoi(line)); break;
				case 3: rcw = sf::Keyboard::Key(stoi(line)); break;
				case 4: rccw = sf::Keyboard::Key(stoi(line)); break;
				case 5: r180 = sf::Keyboard::Key(stoi(line)); break;
				case 6: hd = sf::Keyboard::Key(stoi(line)); break;
				case 7: chat = sf::Keyboard::Key(stoi(line)); break;
				case 8: score = sf::Keyboard::Key(stoi(line)); break;
				case 9: away = sf::Keyboard::Key(stoi(line)); break;
				case 10: ghostpiece = stoi(line); break;
				case 11: fullscreen = stoi(line); break;
				case 12: repeatDelay = sf::milliseconds(stoi(line)); break;
				case 13: repeatSpeed = sf::milliseconds(stoi(line)); break;
				case 14: piecerotation[0] = stoi(line); break;
				case 15: piecerotation[1] = stoi(line); break;
				case 16: piecerotation[2] = stoi(line); break;
				case 17: piecerotation[3] = stoi(line); break;
				case 18: piecerotation[4] = stoi(line); break;
				case 19: piecerotation[5] = stoi(line); break;
				case 20: piecerotation[6] = stoi(line); break;
				case 21: name = line; break;
				case 22: currentmode = stoi(line); break;
				case 23: MusicVolume = stoi(line); break;
				case 24: EffectVolume = stoi(line); break;
				case 25: ChatVolume = stoi(line); break;
				case 26: sound = stoi(line); break;
				case 27: repeatDelayDown = sf::milliseconds(stoi(line)); break;
				case 28: repeatSpeedDown = sf::milliseconds(stoi(line)); break;
				case 29: frameDelay = sf::milliseconds(stoi(line)); break;
				case 30: inputDelay = sf::microseconds(stoi(line)); break;
				case 31: vSync = stoi(line); break;
			}
			countset++;
		}
		file.close();
	}
	else
		success = 0;

	if (countset!=32)
		success = 0;

	if (!success) {
		cout << "loadOptions failed, setting default options" << endl;
		down = sf::Keyboard::K;
		left = sf::Keyboard::J;
		right = sf::Keyboard::L;
		rcw = sf::Keyboard::F;
		rccw = sf::Keyboard::D;
		r180 = sf::Keyboard::A;
		hd = sf::Keyboard::Space;
		chat = sf::Keyboard::LControl;
		score = sf::Keyboard::LShift;
		away = sf::Keyboard::F4;

		ghostpiece = true;
		fullscreen = false;
		vSync = false;

		repeatDelay = sf::milliseconds(150);
		repeatSpeed = sf::milliseconds(0);

		repeatDelayDown = sf::milliseconds(20);
		repeatSpeedDown = sf::milliseconds(20);

		frameDelay = sf::milliseconds(10);
		inputDelay = sf::milliseconds(5);

		for (int x=0; x<7; x++)
			piecerotation[x] = 0;

		name="Player";
		currentmode=-1;
		MusicVolume=100;
		EffectVolume=100;
		ChatVolume=100;
		sound=true;
	}
}

short optionSet::saveOptions() {
	ofstream file("options.cfg", ios::trunc);

	cout << "Saving options..." << endl;

	if (file.is_open()) {
		file << down << endl;
		file << left << endl;
		file << right << endl;
		file << rcw << endl;
		file << rccw << endl;
		file << r180 << endl;
		file << hd << endl;
		file << chat << endl;
		file << score << endl;
		file << away << endl;
		file << ghostpiece << endl;
		file << fullscreen << endl;
		file << repeatDelay.asMilliseconds() << endl;
		file << repeatSpeed.asMilliseconds() << endl;
		for (int x=0; x<7; x++)
			file << piecerotation[x] << endl;
		file << name.toAnsiString() << endl;
		file << currentmode << endl;
		file << MusicVolume << endl;
		file << EffectVolume << endl;
		file << ChatVolume << endl;
		file << sound << endl;
		file << repeatDelayDown.asMilliseconds() << endl;
		file << repeatSpeedDown.asMilliseconds() << endl;
		file << frameDelay.asMilliseconds() << endl;
		file << inputDelay.asMicroseconds() << endl;
		file << vSync;
	}
	else
		cout << "Failed" << endl;
}

void optionSet::initBasePieces() {
	short value[112] = { 0, 4, 0, 0,
						 0, 4, 0, 0,
						 0, 4, 4, 0,
						 0, 0, 0, 0,

						 0, 3, 0, 0,
						 0, 3, 0, 0,
						 3, 3, 0, 0,
						 0, 0, 0, 0,

						 0, 5, 0, 0,
						 0, 5, 5, 0,
						 0, 0, 5, 0,
						 0, 0, 0, 0,

						 0, 7, 0, 0,
						 7, 7, 0, 0,
						 7, 0, 0, 0,
						 0, 0, 0, 0,

						 0, 2, 0, 0,
						 0, 2, 0, 0,
						 0, 2, 0, 0,
						 0, 2, 0, 0,

						 0, 0, 0, 0,
						 1, 1, 1, 0,
						 0, 1, 0, 0,
						 0, 0, 0, 0,

						 0, 0, 0, 0,
						 0, 6, 6, 0,
						 0, 6, 6, 0,
						 0, 0, 0, 0 };

	short vc=0;

	for (int p=0; p<7; p++) {
		basepiece[p].posX=0;
		basepiece[p].posY=0;
		basepiece[p].lpiece=false;
		basepiece[p].rotation=piecerotation[p];
		for (int y=0; y<4; y++)
			for (int x=0; x<4; x++) {
				basepiece[p].grid[y][x] = value[vc];
				if (value[vc])
					basepiece[p].tile=value[vc];
				vc++;
			}
	}
	basepiece[4].lpiece=true;
	basepiece[6].lpiece=true;
}

void optionSet::getVideoModes() {
	std::vector<sf::VideoMode> allmodes = sf::VideoMode::getFullscreenModes();
	bool add=true;
    for (std::size_t i = 0; i < allmodes.size(); ++i) {
    	for (std::size_t p = 0; p<modes.size(); p++) {
    		if (allmodes[i].width == modes[p].width && allmodes[i].height == modes[p].height)
    			add=false;
    	}
    	if (add)
    		modes.push_back(allmodes[i]);
    	add=true;
    }
}

void optionSet::setDelay(short i, sf::String string) {
	short value;
	if (string.getSize() > 0)
		value = stoi(string.toAnsiString());
	else
		value = 0;
	if (i == 1)
		repeatDelay = sf::milliseconds(value);
	else if (i == 2)
		repeatSpeed = sf::milliseconds(value);
	else if (i == 3)
		repeatDelayDown = sf::milliseconds(value);
	else if (i == 4)
		repeatSpeedDown = sf::milliseconds(value);
	else if (i == 5)
		if (value)
			frameDelay = sf::milliseconds(1000/value);
	else if (i == 6)
		inputDelay = sf::microseconds(value);
}