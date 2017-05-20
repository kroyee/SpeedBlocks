#include "gameField.h"
#include "textures.h"
#include <SFML/Graphics.hpp>
#include <iostream>
using std::cout;
using std::endl;
using std::to_string;

gameField::gameField(Resources& _resources) {
    nameTag.setFont(_resources.gfx.typewriter);
    nameTag.setCharacterSize(32);
    texture.create(440, 600);
    sprite.setTexture(texture.getTexture());
    tile = _resources.gfx.tile;
    background = &_resources.gfx.fieldBackground;
}

gameField::gameField(const gameField& field) {
    texture.create(440, 600);
    sprite.setTexture(texture.getTexture());
    tile = field.tile;
    background = field.background;
    name = field.name;
    nameTag = field.nameTag;
    for (int y=0; y<22; y++)
        for (int x=0; x<10; x++)
            square[y][x] = field.square[y][x];
}

void gameField::clear() {
    for (int y=0; y<22; y++)
        for (int x=0; x<10; x++)
            square[y][x] = 0;
    return;
}

void gameField::drawField() {
    texture.clear(sf::Color(255,255,255,0));
    texture.draw(*background);

    drawSquares();

    texture.draw(nameTag);
    texture.display();
}

void gameField::drawSquares() {
    for (int y=4; y<22; y++)
        for (int x=0; x<10; x++)
            if (square[y][x] != 0) {
                tile[square[y][x]-1].setPosition(sf::Vector2f(5+x*30, 5+(y-4)*30));
                texture.draw(tile[square[y][x]-1]);
            }
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

void gameField::setName(const sf::String& n) {
    name = n;
    nameTag.setString(n);
    short x = (310-nameTag.getLocalBounds().width)/2;
    if (x<0)
        x=0;
    nameTag.setPosition(x, 555);
}

void obsField::drawField() {
    texture.clear(sf::Color(255,255,255,0));
    texture.draw(*background);
    
    drawSquares();
    drawPiece();
    drawGhostPiece();
    drawText();
    texture.display();
}

void obsField::drawPiece() {
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

void obsField::drawGhostPiece() {
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

bool obsField::possible() {
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

#define PI 3.14159265
bool obsField::setComboTimer(sf::Uint8 count) {
    if (count>100)
        count=100;
    if (comboTimer.getPointCount() == static_cast<unsigned int>(count+2))
        return false;
    comboTimer.setPointCount(count+2);

    comboTimer.setPoint(0, sf::Vector2f(60, 60));
    for (int x=1; x<(count+2); x++)
        comboTimer.setPoint(x, sf::Vector2f(60 + 60*cos((PI*2)/100 * (x-26)), 60 + 60*sin((PI*2)/100 * (x-26) )));

    return true;
}

void obsField::drawText() {
    if (position == 0)
        positionText.setString("");
    else if (position == 1)
        positionText.setString("1st");
    else if (position == 2)
        positionText.setString("2nd");
    else if (position == 3)
        positionText.setString("3rd");
    else
        positionText.setString(to_string((int)position) + "th");

    if (away)
        awayText.setString("Away");
    else
        awayText.setString("");

    texture.draw(awayText);
    texture.draw(positionText);
    texture.draw(nameTag);
    texture.draw(comboTimer);
    texture.draw(comboText);
    texture.draw(pendingText);
    texture.draw(bpmText);
}

void obsField::updatePiece() {
    for (int x=0; x<4; x++)
        for (int y=0; y<4; y++) {
            if (resources.options.basepiece[piece.piece].grid[y][x])
                piece.grid[y][x] = piece.tile;
            else
                piece.grid[y][x] = 0;
        }
    piece.current_rotation = 0;
    while (piece.current_rotation != piece.rotation)
        piece.rcw();
}

obsField::obsField(const obsField& field) : gameField(field), resources(field.resources) {
    id=field.id; nextpiece=field.nextpiece; nprot=field.nprot; npcol=field.npcol; mouseover=0; away=false; position=0;
    positionText=field.positionText; awayText=field.awayText;
    comboText=field.comboText; pendingText=field.pendingText; bpmText=field.bpmText;

    comboTimer.setPosition(315, 240);
    comboTimer.setFillColor(sf::Color(255,0,0));

    setComboTimer(0);

    piece.piece = 7;
}

obsField::obsField(Resources& _resources) : gameField(_resources), resources(_resources) {
    id=0; nextpiece=0; nprot=0; scale=0; npcol=1; mouseover=0; piece.posX=0; piece.posY=0; away=false; position=0;

    awayText.setFont(_resources.gfx.printFont);
    awayText.setCharacterSize(48);
    awayText.setPosition(110, 150);
    positionText.setFont(_resources.gfx.printFont);
    positionText.setCharacterSize(48);
    positionText.setPosition(130, 220);

    comboText.setFont(_resources.gfx.typewriter); comboText.setString("0");
    pendingText.setFont(_resources.gfx.typewriter); pendingText.setString("0");
    bpmText.setFont(_resources.gfx.typewriter); bpmText.setString("0");
    comboText.setCharacterSize(48);
    comboText.setColor(sf::Color::White);
    comboText.setPosition(360,270);
    pendingText.setCharacterSize(48);
    pendingText.setColor(sf::Color::White);
    pendingText.setPosition(360,500);
    bpmText.setCharacterSize(48);
    bpmText.setColor(sf::Color::White);
    bpmText.setPosition(360, 400);

    comboTimer.setPosition(315, 240);
    comboTimer.setFillColor(sf::Color(255,0,0));

    piece.piece=7;
}