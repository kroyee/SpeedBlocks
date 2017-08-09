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
    sf::Keyboard::Key chat; // Show chat
    sf::Keyboard::Key score; // Show score
    sf::Keyboard::Key away; // Go away
    sf::Keyboard::Key ready; // Ready up

    bool ghostpiece;
    bool fullscreen;
    bool sound;
    bool vSync;
    bool performanceOutput;
    bool animatedBackground;

    short MusicVolume;
    short EffectVolume;
    short ChatVolume;

    sf::Time repeatDelay;
    sf::Time repeatSpeed;

    sf::Time repeatDelayDown;
    sf::Time repeatSpeedDown;

    sf::Time frameDelay;
    sf::Time inputDelay;

    sf::Uint8 piecerotation[7];
    sf::Uint8 colormap[7];
    basePieces basepiece[7];

    sf::Uint8 theme;
    sf::Uint8 ghostPieceAlpha;

    sf::String name;

    std::vector<sf::VideoMode> modes;
    short currentmode;

    void getVideoModes();
    void initBasePieces();
    void loadOptions();
    void loadStandardOptions();
    void saveOptions();
    std::vector<short> pieceArray();

    void setPieceColor(short, sf::Uint8);
    void setDelay(short, sf::String);
};

#endif
