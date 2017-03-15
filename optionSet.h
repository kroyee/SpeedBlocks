#ifndef OPTIONSET_H
#define OPTIONSET_H

#include <SFML/Graphics.hpp>
#include "pieces.h"

class optionSet {
public:
    optionSet() { getVideoModes(); loadOptions(); initBasePieces(); }

    sf::Keyboard::Key up;
    sf::Keyboard::Key down;
    sf::Keyboard::Key left;
    sf::Keyboard::Key right;
    sf::Keyboard::Key rcw; // Rotate clockwise
    sf::Keyboard::Key rccw; // Rotate counter-clockwise
    sf::Keyboard::Key r180; // Rotate 180 degrees
    sf::Keyboard::Key hd; // Hard Drop
    sf::Keyboard::Key chat; // Activate chat
    sf::Keyboard::Key score; // Show score
    sf::Keyboard::Key away; // Go away

    bool ghostpiece;
    bool fullscreen;
    bool sound;
    bool vSync;

    short MusicVolume;
    short EffectVolume;
    short ChatVolume;

    sf::Time repeatDelay;
    sf::Time repeatSpeed;

    sf::Time repeatDelayDown;
    sf::Time repeatSpeedDown;

    sf::Time frameDelay;
    sf::Time inputDelay;

    short piecerotation[7];
    basePieces basepiece[7];

    sf::String name;

    std::vector<sf::VideoMode> modes;
    short currentmode;

    void getVideoModes();
    void initBasePieces();
    short loadOptions();
    short saveOptions();

    void setDelay(short, sf::String);
};

#endif