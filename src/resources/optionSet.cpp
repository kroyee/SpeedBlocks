#include "optionSet.h"
#include <iostream>
#include <fstream>
#include <string>
#include <SFML/Graphics.hpp>
using std::cout;
using std::endl;
using std::stoi;
using std::ifstream;
using std::ofstream;

#ifdef __APPLE__
#include "ResourcePath.hpp"
#include <sys/types.h>
#include <sys/stat.h>
#elif __WIN32
#include <windows.h>
#include <shlobj.h>
#include <stdlib.h>
#endif


void optionSet::loadStandardOptions() {
	down = sf::Keyboard::K;
	left = sf::Keyboard::J;
	right = sf::Keyboard::L;
	rcw = sf::Keyboard::F;
	rccw = sf::Keyboard::D;
	r180 = sf::Keyboard::A;
	hd = sf::Keyboard::Space;
	menu = sf::Keyboard::LShift;
	score = sf::Keyboard::Tab;
	away = sf::Keyboard::F4;
	ready = sf::Keyboard::F5;

	ghostpiece = true;
	fullscreen = false;
	vSync = false;
	performanceOutput = true;
	animatedBackground = true;

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
    colormap[3] = 7;
    colormap[4] = 2;
    colormap[5] = 1;
    colormap[6] = 6;

	name="Player";
	currentmode=0;
	MusicVolume=100;
	EffectVolume=100;
	ChatVolume=100;
	sound=true;

	theme=1;
	ghostPieceAlpha=180;
	hash="";
	rememberme=false;
	noSound=false;
	username="";
	pass=0;

	fieldBackground=0;
	lineStyle=3;
	lineColor=false;
	fieldVLines=false;
	fieldHLines=false;
	mouseMenu=true;
}


void optionSet::loadOptions() {
	std::string line, keyword, pieceoption;
	int pieceindex;

	loadStandardOptions();

	#ifdef __WIN32
	wchar_t* appdataW[1000];
	SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, NULL, appdataW);
	char appdata[500];
	wcstombs(appdata, *appdataW, 500);
	std::string appdataFolder(appdata);
	appdataFolder += "\\SpeedBlocks\\";
	CoTaskMemFree(appdataW[0]);
	CoTaskMemFree(appdataW);

	ifstream file (appdataFolder + "options.cfg");
	#elif __APPLE__
	ifstream file (applicationSupportFolder() + "options.cfg");
	#else
	ifstream file ("options.cfg");
	#endif

	int countset = 0;
	bool success = 1;

	if (file.is_open()) {
		while (getline(file, line)) {
			countset++;
			keyword = line.substr(0, line.find('='));
			line = line.substr(line.find('=')+1);
			if (keyword.find('_') != std::string::npos)
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
			else if (keyword == "menu") menu = sf::Keyboard::Key(stoi(line));
			else if (keyword == "score") score = sf::Keyboard::Key(stoi(line));
			else if (keyword == "away") away = sf::Keyboard::Key(stoi(line));
			else if (keyword == "ready") ready = sf::Keyboard::Key(stoi(line));
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
			else if (keyword == "performanceOutput") performanceOutput = stoi(line);
			else if (keyword == "animatedBackground") animatedBackground = stoi(line);
			else if (keyword == "theme") theme = stoi(line);
			else if (keyword == "ghostPieceAlpha") ghostPieceAlpha = stoi(line);
			else if (keyword == "hash") hash = line;
			else if (keyword == "rememberme") rememberme=stoi(line);
			else if (keyword == "username") username=line;
			else if (keyword == "pass") pass=stoi(line);
			else if (keyword == "fieldBackground") fieldBackground=stoi(line);
			else if (keyword == "fieldVLines") fieldVLines=stoi(line);
			else if (keyword == "fieldHLines") fieldHLines=stoi(line);
			else if (keyword == "lineStyle") lineStyle=stoi(line);
			else if (keyword == "lineColor") lineColor=stoi(line);
			else if (keyword == "mouseMenu") mouseMenu=stoi(line);
			else if (keyword == "noSound") noSound=stoi(line);
			else countset--;
		}
		file.close();
	}
	else
		success = 0;

	if (countset!=55)
		success = 0;

	if (!success)
		cout << "loadOptions failed, missing options set to default" << endl;
}

void optionSet::saveOptions() {
	#ifdef __WIN32
	wchar_t* appdataW[1000];
	SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, NULL, appdataW);
	char appdata[500];
	wcstombs(appdata, *appdataW, 500);
	std::string appdataFolder(appdata);
	appdataFolder += "\\SpeedBlocks\\";
	CreateDirectory(appdataFolder.c_str(), NULL);
	CoTaskMemFree(appdataW[0]);
	CoTaskMemFree(appdataW);

	ofstream file(appdataFolder + "options.cfg");
	#elif __APPLE__
	mkdir(applicationSupportFolder().c_str(), 0755);
	ofstream file (applicationSupportFolder() + "options.cfg");
	#else
	ofstream file("options.cfg");
	#endif
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
		file << "menu=" << menu << endl;
		file << "score=" << score << endl;
		file << "away=" << away << endl;
		file << "ready=" << ready << endl;
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
		file << "vsync=" << vSync << endl;
		file << "performanceOutput=" << performanceOutput << endl;
		file << "animatedBackground=" << animatedBackground << endl;
		file << "theme=" << (int)theme << endl;
		file << "ghostPieceAlpha=" << (int)ghostPieceAlpha << endl;
		file << "rememberme=" << rememberme << endl;
		file << "hash=" << hash.toAnsiString() << endl;
		file << "username=" << username.toAnsiString() << endl;
		file << "pass=" << pass << endl;
		file << "fieldBackground=" << (int)fieldBackground << endl;
		file << "fieldVLines=" << fieldVLines << endl;
		file << "fieldHLines=" << fieldHLines << endl;
		file << "lineStyle=" << (int)lineStyle << endl;
		file << "lineColor=" << lineColor << endl;
		file << "mouseMenu=" << mouseMenu << endl;
		file << "noSound=" << noSound << endl;
	}
	else
		cout << "Failed" << endl;
}

void optionSet::initBasePieces() {
	std::vector<short> value = pieceArray();

	short vc=0;

	for (int p=0; p<7; p++) {
		basepiece[p].posX=0;
		basepiece[p].posY=0;
		basepiece[p].lpiece=false;
		basepiece[p].rotation=piecerotation[p];
		basepiece[p].current_rotation=0;
		basepiece[p].tile=colormap[p];
		basepiece[p].piece=p;
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
}

std::vector<short> optionSet::pieceArray() {
	std::vector<short> value = { 0, 4, 0, 0,
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

	return value;
}