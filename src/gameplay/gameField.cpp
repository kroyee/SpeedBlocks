#include "gameField.h"
#include "Resources.h"
#include <SFML/Graphics.hpp>
#include <iostream>
using std::cout;
using std::endl;
using std::to_string;

gameField::gameField(Resources& _resources) : resources(_resources), text(_resources) {
    texture.create(440, 600);
    text.texture = &texture;
    sprite.setTexture(texture.getTexture());
    tile = resources.gfx.tile;

    piece.piece=7;
}

gameField::gameField(const gameField& field) : resources(field.resources), text(field.resources) {
    texture.create(440, 600);
    text.texture = &texture;
    sprite.setTexture(texture.getTexture());
    tile = field.tile;

    piece.piece = 7;

    for (int y=0; y<22; y++)
        for (int x=0; x<10; x++)
            square[y][x] = field.square[y][x];
}

void gameField::clear() {
    for (int y=0; y<22; y++)
        for (int x=0; x<10; x++)
            square[y][x] = 0;
    text.clear();
    piece.piece=7;
}

void gameField::drawField() {
    texture.clear(sf::Color(255,255,255,0));
    texture.draw(resources.gfx.fieldBackground);
    
    drawSquares();
    drawPiece();
    drawGhostPiece();
    text.drawText();
}

void gameField::drawSquares() {
    for (int y=4; y<22; y++)
        for (int x=0; x<10; x++)
            if (square[y][x] != 0) {
                tile[square[y][x]-1].setPosition(sf::Vector2f(5+x*30, 5+(y-4)*30));
                texture.draw(tile[square[y][x]-1]);
            }
}

void gameField::drawPiece() {
    if (piece.piece == 7)
        return;
    for (int y=0; y<4; y++)
        for (int x=0; x<4; x++)
            if (piece.grid[y][x] != 0)
                if (piece.posY+y > 3) {
                    tile[piece.tile-1].setPosition(sf::Vector2f(5+(piece.posX+x)*30, 5+(piece.posY+y-4)*30));
                    texture.draw(tile[piece.tile-1]);
                }
}

void gameField::drawGhostPiece() {
    if (piece.piece == 7)
        return;
    if (resources.options.ghostpiece) {
        short posY = piece.posY;
        while (possible()) { piece.mdown(); }
        piece.mup();
        for (int y=0; y<4; y++)
            for (int x=0; x<4; x++)
                if (piece.grid[y][x] != 0)
                    if (piece.posY+y > 3) {
                        tile[piece.tile+7].setPosition(sf::Vector2f(5+(piece.posX+x)*30, 5+(piece.posY+y-4)*30));
                        texture.draw(tile[piece.tile+7]);
                    }

        piece.posY = posY;
    }
}

bool gameField::possible() {
    for (int x=0; x<4; x++)
        for (int y=0; y<4; y++)
            if (piece.grid[y][x]) {
                if (piece.posX+x<0 || piece.posX+x>9 || piece.posY+y<0 || piece.posY+y>21)
                    return false;
                if (square[piece.posY+y][piece.posX+x])
                    return false;
            }
    return true;
}

bool gameField::mRight() {
    piece.mright();
    if (possible())
        return true;

    piece.mleft();
    return false;
}

bool gameField::mLeft() {
    piece.mleft();
    if (possible())
        return true;
    
    piece.mright();
    return false;
}

bool gameField::mDown() {
    piece.mdown();
    if (possible())
        return true;

    piece.mup();
    return false;
}

void gameField::hd() {
    while (possible()) { piece.mdown(); }
    piece.mup();
}

bool gameField::rcw() {
    piece.rcw();
    if (possible())
        return true;
    if (kickTest())
        return true;

    piece.posX-=2;
    piece.rccw();
    return false;
}

bool gameField::rccw() {
    piece.rccw();
    if (possible())
        return true;
    if (kickTest())
        return true;

    piece.posX-=2;
    piece.rcw();
    return false;
}

bool gameField::r180() {
    piece.rccw();
    piece.rccw();
    if (possible())
        return true;
    if (kickTest())
        return true;

    piece.posX-=2;
    piece.rcw();
    piece.rcw();
    return false;
}

bool gameField::kickTest() {
    piece.posX--; if (possible()) return true;
    piece.posX+=2; if (possible()) return true;
    piece.posX--; piece.posY++; if (possible()) return true;
    piece.posX--; if (possible()) return true;
    piece.posX+=2; if (possible()) return true;
    piece.posX-=3; piece.posY--; if (possible()) return true;
    piece.posX+=4; if (possible()) return true;

    return false;
}

void gameField::addPiece() {
    for (int x=0; x<4; x++)
        for (int y=0; y<4; y++)
            if (piece.grid[y][x])
                square[piece.posY+y][piece.posX+x]=piece.tile;
}

void gameField::removeline(short y) {
    for (;y>-1; y--)
        for (int x=0; x<10; x++)
            square[y][x] = square[y-1][x];
    for (int x=0; x<10; x++)
        square[0][x] = 0;
}

sf::Vector2i gameField::clearlines () {
    sf::Vector2i linescleared(0,0);
    bool rm, gb;
    for (int y=21; y>-1; y--) {
        rm=1;
        gb=0;
        for (int x=0; x<10; x++) {
            if (square[y][x] == 8)
                gb=1;
            if (square[y][x] == 0) {
                rm=0;
                break;
            }
        }
        if (rm) {
            removeline(y);
            y++;
            linescleared.x++;
            if (gb)
                linescleared.y++;
        }
    }
    return linescleared;
}

void obsField::drawField() {
    texture.clear(sf::Color(255,255,255,0));
    texture.draw(resources.gfx.fieldBackground);
    
    drawSquares();
    drawPiece();
    drawGhostPiece();
    drawNextPiece();
    text.drawText();
    texture.display();
}

void obsField::drawNextPiece() {
    for (int rot=0; rot < nprot; rot++)
        resources.options.basepiece[nextpiece].rcw();
    for (int y=0; y<4; y++)
        for (int x=0; x<4; x++)
            if (resources.options.basepiece[nextpiece].grid[y][x] != 0) {
                    tile[npcol-1].setPosition(sf::Vector2f(-15*resources.options.basepiece[nextpiece].lpiece+330+x*30, 45+y*30));
                    texture.draw(tile[npcol-1]);
                }
    for (int rot=0; rot < nprot; rot++)
        resources.options.basepiece[nextpiece].rccw();
}

void obsField::updatePiece() {
    for (int x=0; x<4; x++)
        for (int y=0; y<4; y++) {
            if (resources.options.basepiece[piece.piece].grid[y][x])
                piece.grid[y][x] = piece.tile;
            else
                piece.grid[y][x] = 0;
        }
    piece.lpiece = resources.options.basepiece[piece.piece].lpiece;
    piece.current_rotation = 0;
    while (piece.current_rotation != piece.rotation)
        piece.rcw();
}

obsField::obsField(const obsField& field) : gameField(field) {
    id=field.id; nextpiece=field.nextpiece; nprot=field.nprot; npcol=field.npcol; mouseover=0;
}

obsField::obsField(Resources& _resources) : gameField(_resources) {
    id=0; nextpiece=0; nprot=0; scale=0; npcol=1; mouseover=0; piece.posX=0; piece.posY=0;
}