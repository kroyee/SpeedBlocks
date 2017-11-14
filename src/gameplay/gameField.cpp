#include "gameField.h"
#include "Resources.h"
#include "textures.h"
#include "optionSet.h"
#include "GameSignals.h"
#include <SFML/Graphics.hpp>
#include <iostream>
using std::cout;
using std::endl;
using std::to_string;

BasicField::BasicField(Resources& _resources) : resources(_resources) {}

BasicField::BasicField(const BasicField& field) : resources(field.resources) {}

bool BasicField::possible() {
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

bool BasicField::mRight() {
    piece.mright();
    if (possible())
        return true;

    piece.mleft();
    return false;
}

bool BasicField::mLeft() {
    piece.mleft();
    if (possible())
        return true;
    
    piece.mright();
    return false;
}

bool BasicField::mDown() {
    piece.mdown();
    if (possible())
        return true;

    piece.mup();
    return false;
}

void BasicField::hd() {
    do { piece.mdown(); }
    while (possible());
    piece.mup();
}

bool BasicField::rcw() {
    piece.rcw();
    if (possible())
        return true;
    if (kickTest())
        return true;

    piece.posX-=2;
    piece.rccw();
    return false;
}

bool BasicField::rccw() {
    piece.rccw();
    if (possible())
        return true;
    if (kickTest())
        return true;

    piece.posX-=2;
    piece.rcw();
    return false;
}

bool BasicField::r180() {
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

bool BasicField::kickTest() {
    piece.posY++; if (possible()) return true;
    piece.posY--; piece.posX--; if (possible()) return true;
    piece.posX+=2; if (possible()) return true;
    piece.posY++; piece.posX-=2; if (possible()) return true;
    piece.posX+=2; if (possible()) return true;
    piece.posX-=3; piece.posY--; if (possible()) return true;
    piece.posX+=4; if (possible()) return true;

    return false;
}

void BasicField::addPiece() {
    for (int x=0; x<4; x++)
        for (int y=0; y<4; y++)
            if (piece.grid[y][x])
                square[piece.posY+y][piece.posX+x]=piece.tile;
}

void BasicField::removeline(short y) {
    for (;y>-1; y--)
        for (int x=0; x<10; x++)
            square[y][x] = square[y-1][x];
    for (int x=0; x<10; x++)
        square[0][x] = 0;
}

sf::Vector2i BasicField::clearlines () {
    sf::Vector2i linescleared(0,0);
    bool rm, gb;
    for (int y=21; y>-1; y--) {
        if (piece.posY+y > 21 || piece.posY+y < 0)
            continue;
        rm=1;
        gb=0;
        for (int x=0; x<10; x++) {
            if (square[piece.posY+y][x] == 8)
                gb=1;
            if (square[piece.posY+y][x] == 0) {
                rm=0;
                break;
            }
        }
        if (rm) {
            removeline(piece.posY+y);
            y++;
            linescleared.x++;
            if (gb)
                linescleared.y++;
        }
    }
    return linescleared;
}

gameField::gameField(Resources& _resources) : BasicField(_resources), text(_resources) {
    texture.create(440, 600);
    text.texture = &texture;
    sprite.setTexture(texture.getTexture());
    tile = resources.gfx->tile;
    backRect.setPosition(5,5);
    backRect.setSize({300,540});
    setBackColor(resources.options->fieldBackground);

    piece.piece=7;
    pieceCopy.piece=7;
    offset=0;
    status=0;

    for (int y=0; y<22; y++)
        for (int x=0; x<10; x++) {
            square[y][x] = 0;
            squareCopy[y][x] = 0;
        }
}

gameField::gameField(const gameField& field) : BasicField(field.resources), text(field.resources) {
    texture.create(440, 600);
    text.texture = &texture;
    sprite.setTexture(texture.getTexture());
    tile = field.tile;
    backRect.setPosition(5,5);
    backRect.setSize({300,540});
    setBackColor(resources.options->fieldBackground);

    piece.piece = 7;
    pieceCopy.piece=7;
    offset=0;
    status=0;

    for (int y=0; y<22; y++)
        for (int x=0; x<10; x++)
            square[y][x] = field.square[y][x];
}

void gameField::clear() {
    for (int y=0; y<22; y++)
        for (int x=0; x<10; x++) {
            square[y][x] = 0;
            squareCopy[y][x] = 0;
        }
    text.clear();
    piece.piece=7;
    pieceCopy.piece=7;
    offset=0;
}

bool gameField::possibleCopy() {
    for (int x=0; x<4; x++)
        for (int y=0; y<4; y++)
            if (pieceCopy.grid[y][x]) {
                if (pieceCopy.posX+x<0 || pieceCopy.posX+x>9 || pieceCopy.posY+y<0 || pieceCopy.posY+y>21)
                    return false;
                if (square[pieceCopy.posY+y][pieceCopy.posX+x])
                    return false;
            }
    return true;
}

void gameField::drawField(bool drawLines) {
    texture.clear(sf::Color(255,255,255,0));
    texture.draw(backRect);
    if (drawLines)
        texture.draw(background);
    drawEdges();
    drawSquares();
    drawPiece();
    drawGhostPiece();
    text.drawText();
}

void gameField::drawEdges() {
    sf::RectangleShape rect;
    rect.setFillColor(sf::Color(127,127,127,200));
    rect.setPosition(0,0);
    rect.setSize({310,5});
    texture.draw(rect);
    rect.setPosition(0,545);
    texture.draw(rect);
    rect.setPosition(0,5);
    rect.setSize({5,540});
    texture.draw(rect);
    rect.setPosition(305,5);
    texture.draw(rect);
}

void gameField::drawTile(sf::Uint8 color, sf::Uint8 x, sf::Uint8 y) {
    if (y<3 || color == 0)
        return;
    color--;

    if (y == 3) {
        if (!offset)
            return;
        tile[color].setTextureRect({0,30-offset,30,offset});
        tile[color].setPosition(sf::Vector2f(5+x*30, 5));
        texture.draw(tile[color]);
        tile[color].setTextureRect({0,0,30,30});
    }
    else if (y == 21 && offset) {
        tile[color].setTextureRect({0,0,30,30-offset});
        tile[color].setPosition(sf::Vector2f(5+x*30, 515 + offset));
        texture.draw(tile[color]);
        tile[color].setTextureRect({0,0,30,30});
    }
    else {
        tile[color].setPosition(sf::Vector2f(5+x*30, 5+(y-4)*30 + offset));
        texture.draw(tile[color]);
    }
}

void gameField::drawSquares() {
    for (int y=3; y<22; y++)
        for (int x=0; x<10; x++)
            drawTile(squareCopy[y][x], x, y);
}

void gameField::drawPiece() {
    if (pieceCopy.piece == 7)
        return;
    for (int y=0; y<4; y++)
        for (int x=0; x<4; x++)
            if (pieceCopy.grid[y][x] != 0)
                drawTile(pieceCopy.tile, pieceCopy.posX+x, pieceCopy.posY+y);
}

void gameField::drawGhostPiece() {
    if (pieceCopy.piece == 7)
        return;
    if (resources.options->ghostpiece) {
        short posY = pieceCopy.posY;
        while (possibleCopy()) { pieceCopy.mdown(); }
        pieceCopy.mup();
        for (int y=0; y<4; y++)
            for (int x=0; x<4; x++)
                if (pieceCopy.grid[y][x] != 0)
                    drawTile(pieceCopy.tile+8, pieceCopy.posX+x, pieceCopy.posY+y);

        pieceCopy.posY = posY;
    }
}

void gameField::setBackColor(sf::Uint8 val) {
    backRect.setFillColor(sf::Color(val,val,val,225));
}

void obsField::makeDrawCopy() {
    makeNextPieceCopy();
    squareCopy = square;
    pieceCopy = piece;
    drawMe=false;
    status = 1;
}

void obsField::drawThreadLoop() {
    while (status != 5) {
        if (status == 1) {
            drawField();
            Signals::FieldFinishedDrawing();
            if (status == 5)
                return;
            status = 0;
        }
        sf::sleep(sf::seconds(0));
    }
}

void obsField::drawField() {
    texture.clear(sf::Color(255,255,255,0));
    texture.draw(backRect);
    
    drawEdges();
    drawSquares();
    drawPiece();
    drawGhostPiece();
    drawNextPiece();
    text.drawText();
    texture.display();
}

void obsField::drawNextPiece() {
    for (int y=0; y<4; y++)
        for (int x=0; x<4; x++)
            if (npPiece.grid[y][x] != 0) {
                    tile[npPiece.tile].setPosition(sf::Vector2f(-15*npPiece.lpiece+330+x*30, 45+y*30));
                    texture.draw(tile[npPiece.tile]);
                }
}

void obsField::updatePiece() {
    for (int x=0; x<4; x++)
        for (int y=0; y<4; y++) {
            if (resources.options->basepiece[piece.piece].grid[y][x])
                piece.grid[y][x] = piece.tile;
            else
                piece.grid[y][x] = 0;
        }
    piece.lpiece = resources.options->basepiece[piece.piece].lpiece;
    piece.current_rotation = 0;
    while (piece.current_rotation != piece.rotation)
        piece.rcw();
}

void obsField::makeNextPieceCopy() {
    npPiece = resources.options->basepiece[nextpiece];
    npPiece.tile = npcol-1;
    while (npPiece.current_rotation != nprot)
        npPiece.rcw();
}

void obsField::launchDrawThread() {
    if (!drawThread.joinable()) {
        status = 0;
        drawThread = std::thread(&obsField::drawThreadLoop, this);
    }
}

obsField::obsField(const obsField& field) : gameField(field) {
    id=field.id; nextpiece=field.nextpiece; nprot=field.nprot; npcol=field.npcol; mouseover=0;
}

obsField::obsField(Resources& _resources) : gameField(_resources) {
    id=0; nextpiece=0; nprot=0; scale=0; npcol=1; mouseover=0; piece.posX=0; piece.posY=0;
    drawThread = std::thread(&obsField::drawThreadLoop, this);
}