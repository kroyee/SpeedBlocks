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
    sf::Keyboard::Key menu; // Show menu
    sf::Keyboard::Key score; // Show score
    sf::Keyboard::Key away; // Go away
    sf::Keyboard::Key ready; // Ready up

    bool ghostpiece;
    bool fullscreen;
    bool sound;
    bool vSync;
    bool performanceOutput;
    bool animatedBackground;
    bool rememberme;
    bool noSound;
    sf::String hash;
    sf::String username;
    short pass;

    short MusicVolume;
    short EffectVolume;
    short ChatVolume;

    sf::Time repeatDelay;
    sf::Time repeatSpeed;

    sf::Time repeatDelayDown;
    sf::Time repeatSpeedDown;

    sf::Time frameDelay;
    sf::Time inputDelay;

    uint8_t piecerotation[7];
    uint8_t colormap[7];
    basePieces basepiece[7];

    uint8_t theme;
    uint8_t ghostPieceAlpha;
    uint8_t fieldBackground;
    uint8_t lineStyle;
    bool lineColor;
    bool fieldVLines;
    bool fieldHLines;

    bool mouseMenu;

    sf::String name;

    std::vector<sf::VideoMode> modes;
    short currentmode;

    void getVideoModes();
    void initBasePieces();
    void loadOptions();
    void loadStandardOptions();
    void saveOptions();
    std::vector<short> pieceArray();

    void setPieceColor(short, uint8_t);
    void setDelay(short, sf::String);
};

#endif
