#ifndef GAMEFIELD_H
#define GAMEFIELD_H

#include <SFML/Graphics.hpp>
#include "pieces.h"
#include "GameFieldText.h"
#include <thread>
#include <atomic>
#include <array>

class Resources;

struct PiecePreviewInfo {
	PiecePreviewInfo(uint8_t p, uint8_t r=0, uint8_t c=1, float s=1.f) :
		piece(p), rotation(r), color(c), scale(s) {}
	uint8_t piece, rotation, color;
	float scale;
};

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

class GameField : public BasicField {
public:
    sf::RenderTexture texture;
    sf::Sprite sprite;
    std::array<sf::Sprite, 16>& tile;

    sf::RectangleShape backRect;

    GameFieldText text;

    float offset;
	uint8_t base_offset;

    bool drawMe;

    std::array<std::array<uint8_t, 10>, 22> squareCopy;
    basePieces pieceCopy;

    std::thread drawThread;
    std::atomic<uint8_t> status;

    GameField(Resources& _resources);

    void clear();

    bool possibleCopy();

    void drawTile(uint8_t color, uint8_t x, uint8_t y);

    void drawField();
    void drawEdges();
    void drawSquares();
    void drawPiece();
	void drawPiecePreview(const PiecePreviewInfo& data, sf::Vector2f pos);
    void drawGhostPiece();

    void setBackColor(uint8_t val);
	bool setOffset(uint8_t val);
};

class ObsField : public GameField {
public:
    ObsField(Resources& _resources);

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
