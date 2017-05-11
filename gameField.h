#ifndef GAMEFIELD_H
#define GAMEFIELD_H

#include <SFML/Graphics.hpp>
#include "pieces.h"

class Resources;

class gameField {
public:
    sf::Uint8 square[22][10];

    sf::RenderTexture texture;
    sf::Sprite sprite;
    sf::Sprite* tile;
    sf::Sprite* background;
    sf::Text nameTag;
    sf::Text awayText;
    sf::Text positionText;

    sf::String name;

    gameField(Resources& _resources);
    gameField(const gameField& field);

    void clear();

    void drawField();
    void drawSquares();

    sf::Sprite* getSprite() { return &sprite; }

    void setsquare(short y, short x, short color) { square[y][x] = color; }

    short getsquare(short y, short x) { return square[y][x]; }

    void removeline(short y);

    sf::Vector2i clearlines();

    void setName(const sf::String& n);
};

class obsField : public gameField {
public:
    obsField(const obsField& field);
    
    obsField(Resources& _resources);

    Resources& resources;
    sf::Uint16 id;
    sf::Uint8 nextpiece, nprot, npcol;
    short scale;
    basePieces piece;
    bool mouseover, away;
    sf::Uint8 datacount, position;

    sf::Text comboText, pendingText, bpmText;

    sf::ConvexShape comboTimer;

    void drawField();
    void preDrawField();

    void drawPiece();
    bool setComboTimer(sf::Uint8 count);
    void drawText();
    void drawNextPiece();
    void drawGhostPiece();
    void updatePiece();
    bool possible();
};

#endif