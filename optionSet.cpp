#include "optionSet.h"
#include <iostream>
#include <fstream>
#include <string>
#include <SFML/Graphics.hpp>
#include "MingwConvert.h"
using namespace std;

#ifdef __APPLE__
#include "ResourcePath.hpp"
#else
#include "EmptyResourcePath.h"
#endif


void optionSet::loadStandardOptions() {
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

		//frame delay multiplicator 10 = 100 FPS
		frameDelay = sf::milliseconds(10);
        //input delay multiplicator 1000 = 1000 microseconds - 1ms
		inputDelay = sf::milliseconds(1);

		piecerotation[0] = 3;
		piecerotation[1] = 1;
		piecerotation[2] = 3;
		piecerotation[3] = 1;
		piecerotation[4] = 1;
		piecerotation[5] = 2;
		piecerotation[6] = 0;

        colormap[0] = 4;
        colormap[1] = 3;
        colormap[2] = 5;
        colormap[3] = 8;
        colormap[4] = 2;
        colormap[5] = 1;
        colormap[6] = 6;

		name="Player";
		currentmode=-1;
		MusicVolume=100;
		EffectVolume=100;
		ChatVolume=100;
		sound=true;
}


void optionSet::loadOptions() {
	string line, keyword, pieceoption;
	int pieceindex;

	loadStandardOptions();

	ifstream file (resourcePath() + "options.cfg");

	int countset = 0;
	bool success = 1;

	if (file.is_open()) {
		while (getline(file, line)) {
			countset++;
			keyword = line.substr(0, line.find('='));
			line = line.substr(line.find('=')+1);
			if (keyword.find('_') != string::npos)
			{
					pieceindex = stoi(keyword.substr(keyword.find('_')+1, 1));
					pieceoption = keyword.substr(keyword.find('_', keyword.find('_')+1)+1);
					keyword = keyword.substr(0, keyword.find('_'));
			}
			if (keyword == "down") down = sf::Keyboard::Key(stoi(line));
			else if (keyword == "left") left = sf::Keyboard::Key(stoi(line));
			else if (keyword == "right") right = sf::Keyboard::Key(stoi(line));
			else if (keyword == "rcw") rcw = sf::Keyboard::Key(stoi(line));
			else if (keyword == "rccw") rccw = sf::Keyboard::Key(stoi(line));
			else if (keyword == "r180") r180 = sf::Keyboard::Key(stoi(line));
			else if (keyword == "hd") hd = sf::Keyboard::Key(stoi(line));
			else if (keyword == "chat") chat = sf::Keyboard::Key(stoi(line));
			else if (keyword == "score") score = sf::Keyboard::Key(stoi(line));
			else if (keyword == "away") away = sf::Keyboard::Key(stoi(line));
			else if (keyword == "ghostpiece") ghostpiece = stoi(line);
			else if (keyword == "fullscreen") fullscreen = stoi(line);
			else if (keyword == "repeatdelay") repeatDelay = sf::milliseconds(stoi(line));
			else if (keyword == "repeatspeed") repeatSpeed = sf::milliseconds(stoi(line));
			else if (keyword == "piece")
			{
					if (pieceoption == "rotation") piecerotation[pieceindex] = stoi(line);
					if (pieceoption == "color") colormap[pieceindex] = stoi(line);
			}
			else if (keyword == "name") name = line;
			else if (keyword == "currentmode") currentmode = stoi(line);
			else if (keyword == "musicvolume") MusicVolume = stoi(line);
			else if (keyword == "effectvolume") EffectVolume = stoi(line);
			else if (keyword == "chatvolume") ChatVolume = stoi(line);
			else if (keyword == "sound") sound = stoi(line);
			else if (keyword == "repeatdelaydown") repeatDelayDown = sf::milliseconds(stoi(line));
			else if (keyword == "repeatspeeddown") repeatSpeedDown = sf::milliseconds(stoi(line));
			else if (keyword == "framedelay") frameDelay = sf::microseconds(stoi(line));
			else if (keyword == "inputdelay") inputDelay = sf::microseconds(stoi(line));
			else if (keyword == "vsync") vSync = stoi(line);
			else countset--;
		}
		file.close();
	}
	else
		success = 0;

	if (countset!=39)
		success = 0;

	if (!success)
		cout << "loadOptions failed, missing options set to default" << endl;
}

void optionSet::saveOptions() {
	ofstream file(resourcePath() + "options.cfg", ios::trunc);
    sf::Color col;

	cout << "Saving options..." << endl;

	if (file.is_open()) {
		file << "down=" << down << endl;
		file << "left=" << left << endl;
		file << "right=" << right << endl;
		file << "rcw=" << rcw << endl;
		file << "rccw=" << rccw << endl;
		file << "r180=" << r180 << endl;
		file << "hd=" << hd << endl;
		file << "chat=" << chat << endl;
		file << "score=" << score << endl;
		file << "away=" << away << endl;
		file << "ghostpiece=" << ghostpiece << endl;
		file << "fullscreen=" << fullscreen << endl;
		file << "repeatdelay=" << repeatDelay.asMilliseconds() << endl;
		file << "repeatspeed=" << repeatSpeed.asMilliseconds() << endl;
		for (int x=0; x<7; x++)
		{
			file << "piece_" << x << "_rotation=" << (int)piecerotation[x] << endl;
			file << "piece_" << x << "_color=" << (int)colormap[x] << endl;
        }
		file << "name=" << name.toAnsiString() << endl;
		file << "currentmode=" << currentmode << endl;
		file << "musicvolume=" << MusicVolume << endl;
		file << "effectvolume=" << EffectVolume << endl;
		file << "chatvolume=" << ChatVolume << endl;
		file << "sound=" << sound << endl;
		file << "repeatdelaydown=" << repeatDelayDown.asMilliseconds() << endl;
		file << "repeatspeeddown=" << repeatSpeedDown.asMilliseconds() << endl;
		file << "framedelay=" << frameDelay.asMicroseconds() << endl;
		file << "inputdelay=" << inputDelay.asMicroseconds() << endl;
		file << "vsync=" << vSync;
	}
	else
		cout << "Failed" << endl;
}

void optionSet::initBasePieces() {
	short value[112] = { 0, 1, 0, 0,
						 0, 1, 0, 0,
						 0, 1, 1, 0,
						 0, 0, 0, 0,

						 0, 1, 0, 0,
						 0, 1, 0, 0,
						 1, 1, 0, 0,
						 0, 0, 0, 0,

						 0, 1, 0, 0,
						 0, 1, 1, 0,
						 0, 0, 1, 0,
						 0, 0, 0, 0,

						 0, 1, 0, 0,
						 1, 1, 0, 0,
						 1, 0, 0, 0,
						 0, 0, 0, 0,

						 0, 1, 0, 0,
						 0, 1, 0, 0,
						 0, 1, 0, 0,
						 0, 1, 0, 0,

						 0, 0, 0, 0,
						 1, 1, 1, 0,
						 0, 1, 0, 0,
						 0, 0, 0, 0,

						 0, 0, 0, 0,
						 0, 1, 1, 0,
						 0, 1, 1, 0,
						 0, 0, 0, 0 };

	short vc=0;

	for (int p=0; p<7; p++) {
		basepiece[p].posX=0;
		basepiece[p].posY=0;
		basepiece[p].lpiece=false;
		basepiece[p].rotation=piecerotation[p];
		basepiece[p].tile=colormap[p];
		for (int y=0; y<4; y++)
			for (int x=0; x<4; x++) {
                basepiece[p].grid[y][x] = value[vc];
				vc++;
			}
        setPieceColor(p, basepiece[p].tile);
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

void optionSet::setPieceColor(short i, sf::Uint8 newcolor) {
    colormap[i] = newcolor;
    basepiece[i].tile = newcolor;
	for (int x=0; x<4; x++)
		for (int y=0; y<4; y++)
			if (basepiece[i].grid[y][x])
				basepiece[i].grid[y][x]=basepiece[i].tile;
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
	else if (i == 5) {
		if (value)
			frameDelay = sf::milliseconds(1000/value);
	}
	else if (i == 6)
		inputDelay = sf::microseconds(value);
}
