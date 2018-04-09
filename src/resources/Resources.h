#ifndef RESOURCES_H
#define RESOURCES_H

#include <memory>
#include <list>
#include <SFML/Graphics.hpp>

class AQ;
class optionSet;
class textures;
class soundBank;
class network;
class PacketCompress;

enum class GameStates : unsigned int { MainMenu, CountDown, Game, GameOver, Replay, Practice, Spectating };

struct clientInfo {
public:
	uint16_t id;
	sf::String name;
};

class Resources {
public:
	Resources(sf::RenderWindow& _window);
	~Resources();
	optionSet* options;
	textures* gfx;
	soundBank* sounds;
	network* net;
	PacketCompress* compressor;

	GameStates gamestate;

	AQ* actions;

	sf::RenderWindow& window;

	bool playonline, away, restart, chatFocused;

	sf::Clock delayClock;

	uint16_t myId;

	sf::String name;
	std::list<clientInfo> clientList;

	uint8_t version_major;
	uint8_t version_minor;
	uint8_t version_patch;
	uint16_t clientVersion;

	bool init();
};

#endif