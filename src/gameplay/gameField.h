#ifndef GAMEFIELD_H
#define GAMEFIELD_H

#include <SFML/Graphics.hpp>
#include "pieces.h"
#include "GameFieldText.h"

class Resources;

class gameField {
public:
    sf::Uint8 square[22][10];
    basePieces piece;

    sf::RenderTexture texture;
    sf::Sprite sprite;
    sf::Sprite* tile;

    Resources& resources;

    GameFieldText text;

    gameField(Resources& _resources);
    gameField(const gameField& field);

    void clear();

    void drawField();
    void drawSquares();
    void drawPiece();
    void drawGhostPiece();

    bool possible();

    bool mRight();
    bool mLeft();
    bool mDown();
    void hd();
    bool rcw();
    bool rccw();
    bool r180();
    bool kickTest();

    void addPiece();

    void removeline(short y);

    sf::Vector2i clearlines();
};

class obsField : public gameField {
public:
    obsField(const obsField& field);
    
    obsField(Resources& _resources);

    sf::Uint16 id;
    sf::Uint8 nextpiece, nprot, npcol;
    float scale;
    bool mouseover;
    sf::Uint8 datacount;

    void drawField();

    void drawNextPiece();
    void updatePiece();
};

#endif