#ifndef RESOURCES_H
#define RESOURCES_H

#include <SFML/Graphics.hpp>
#include <list>
#include <memory>

class AQ;
class Textures;
class soundBank;
class network;
class PacketCompress;

enum class GameStates : unsigned int { MainMenu, CountDown, Game, GameOver, Replay, Practice, Spectating };

struct clientInfo {
    uint16_t id;
    std::string name;
};

class Resources {
   public:
    Resources(sf::RenderWindow& _window);
    ~Resources();
    Textures* gfx;
    soundBank* sounds;
    network* net;

    GameStates gamestate;

    AQ* actions;

    sf::RenderWindow& window;

    bool playonline, away, restart, chatFocused;

    sf::Clock delayClock;

    uint16_t myId;

    std::string name;
    std::list<clientInfo> clientList;

    uint8_t version_major;
    uint8_t version_minor;
    uint8_t version_patch;
    uint16_t clientVersion;

    bool init();
};

#endif
