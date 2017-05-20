#ifndef RESOURCES_H
#define RESOURCES_H

#include "textures.h"
#include "network.h"
#include "sounds.h"
#include "optionSet.h"

class Resources {
public:
	optionSet options;
	textures gfx;
	soundBank sounds;
	network net;

	bool init();
};

#endif