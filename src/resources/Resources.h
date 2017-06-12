#ifndef RESOURCES_H
#define RESOURCES_H

#include "textures.h"
#include "network.h"
#include "sounds.h"
#include "optionSet.h"

enum GameStates { MainMenu, CountDown, Game, GameOver, Replay, Practice };

class Resources {
public:
	optionSet options;
	textures gfx;
	soundBank sounds;
	network net;

	GameStates gamestate;

	bool init();
};

#endif