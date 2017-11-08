#ifndef GAMEFIELD_H
#define GAMEFIELD_H

#include <SFML/Graphics.hpp>
#include "pieces.h"
#include "GameFieldText.h"

class Resources;

class BasicField {
public:
    BasicField(Resources& _resources);
    BasicField(const BasicField& field);
    Resources& resources;

    std::array<std::array<sf::Uint8, 10>, 22> square;
    basePieces piece;

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

class gameField : public BasicField {
public:
    sf::RenderTexture texture;
    sf::Sprite sprite;
    sf::Sprite* tile;

    sf::Texture backgroundTexture;
    sf::Sprite background;

    sf::RectangleShape backRect;

    GameFieldText text;

    sf::Uint8 offset;

    gameField(Resources& _resources);
    gameField(const gameField& field);

    void clear();

    void drawTile(sf::Uint8 color, sf::Uint8 x, sf::Uint8 y);

    void drawField(bool drawLines);
    void drawEdges();
    void drawSquares();
    void drawPiece();
    void drawGhostPiece();

    void setBackColor(sf::Uint8 val);
};

class obsField : public gameField {
public:
    obsField(const obsField& field);
    
    obsField(Resources& _resources);

    sf::Uint16 id;
    sf::Uint8 nextpiece, nprot, npcol;
    float scale;
    bool mouseover;
    bool drawMe;
    sf::Uint8 datacount;

    void drawField();

    void drawNextPiece();
    void updatePiece();
};

#endif