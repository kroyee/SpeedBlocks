#ifndef GAMEFIELD_H
#define GAMEFIELD_H

#include <SFML/Graphics.hpp>
#include "pieces.h"
#include "GameFieldText.h"
#include <thread>
#include <atomic>
#include <array>

class Resources;

class BasicField {
public:
    BasicField(Resources& _resources);
    //BasicField(const BasicField& field);
    Resources& resources;

    std::array<std::array<uint8_t, 10>, 22> square;
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
    std::array<sf::Sprite, 16>& tile;

    sf::Texture backgroundTexture;
    sf::Sprite background;

    sf::RectangleShape backRect;

    GameFieldText text;

    uint8_t offset;

    bool drawMe;

    std::array<std::array<uint8_t, 10>, 22> squareCopy;
    basePieces pieceCopy;

    std::thread drawThread;
    std::atomic<uint8_t> status;

    gameField(Resources& _resources);
    //gameField(const gameField& field);

    void clear();

    bool possibleCopy();

    void drawTile(uint8_t color, uint8_t x, uint8_t y);

    void drawField(bool drawLines);
    void drawEdges();
    void drawSquares();
    void drawPiece();
    void drawGhostPiece();

    void setBackColor(uint8_t val);
};

class obsField : public gameField {
public:
    //obsField(const obsField& field);
    
    obsField(Resources& _resources);

    uint16_t id;
    uint8_t nextpiece, nprot, npcol;
    float scale;
    bool mouseover;
    uint8_t datacount;

    basePieces npPiece;

    void drawField();

    void drawNextPiece();
    void updatePiece();
    void makeNextPieceCopy();

    void makeDrawCopy();
};

#endif