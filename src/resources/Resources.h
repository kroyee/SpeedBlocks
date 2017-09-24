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

enum class GameStates : unsigned int { MainMenu, CountDown, Game, GameOver, Replay, Practice, Spectating };

struct clientInfo {
public:
	sf::Uint16 id;
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

	GameStates gamestate;

	AQ* actions;

	sf::RenderWindow& window;

	bool playonline, away, restart;

	sf::Clock delayClock;

	sf::Uint16 myId;

	sf::String name;
	std::list<clientInfo> clientList;

	sf::Uint8 version_major;
	sf::Uint8 version_minor;
	sf::Uint8 version_patch;
	sf::Uint16 clientVersion;

	bool init();
};

#endif