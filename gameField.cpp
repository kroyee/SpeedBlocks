#include "gameField.h"
#include <SFML/Graphics.hpp>
#include <iostream>
#include "MingwConvert.h"
using namespace std;

gameField::gameField(sf::Sprite* tilep, sf::Sprite* backgroundp, sf::Font& font) {
    nameTag.setFont(font);
    nameTag.setCharacterSize(32);
	texture.create(440, 600);
    sprite.setTexture(texture.getTexture());
    tile = tilep;
    background = backgroundp;
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
    for (int y=4; y<22; y++)
        for (int x=0; x<10; x++)
            if (square[y][x] != 0) {
                tile[square[y][x]-1].setPosition(sf::Vector2f(5+x*30, 5+(y-4)*30));
                texture.draw(tile[square[y][x]-1]);
            }
    texture.draw(nameTag);
    texture.display();
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
    for (int y=4; y<22; y++)
        for (int x=0; x<10; x++)
            if (square[y][x] != 0) {
                tile[square[y][x]-1].setPosition(sf::Vector2f(5+x*30, 5+(y-4)*30));
                texture.draw(tile[square[y][x]-1]);
            }

    for (int y=0; y<4; y++)
        for (int x=0; x<4; x++)
            if (grid[y][x] != 0)
                if (posY+y > 3) {
                    tile[grid[y][x]-1].setPosition(sf::Vector2f(5+(posX+x)*30, 5+(posY+y-4)*30));
                    texture.draw(tile[grid[y][x]-1]);
                }

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
    texture.draw(comboText);
    texture.draw(pendingText);
    texture.draw(bpmText);
    texture.display();
}

void obsField::preDrawField() {
    texture.clear(sf::Color(255,255,255,0));
    texture.draw(*background);
    for (int y=4; y<22; y++)
        for (int x=0; x<10; x++)
            if (square[y][x] != 0) {
                tile[square[y][x]-1].setPosition(sf::Vector2f(5+x*30, 5+(y-4)*30));
                texture.draw(tile[square[y][x]-1]);
            }

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
    texture.draw(comboText);
    texture.draw(pendingText);
    texture.draw(bpmText);
    texture.display();
}

obsField::obsField(const obsField& field) : gameField(field) {
    id=field.id; nextpiece=field.nextpiece; nprot=field.nprot; npcol=field.npcol; mouseover=0; away=false; position=0;
    positionText=field.positionText; awayText=field.awayText;
    comboText=field.comboText; pendingText=field.pendingText; bpmText=field.bpmText;
    for (int x=0; x<4; x++)
        for (int y=0; y<4; y++) {
            grid[y][x]=0;
        }
}

obsField::obsField(sf::Sprite* tilep, sf::Sprite* backgroundp, sf::Font& font1, sf::Font& font2) : gameField(tilep, backgroundp, font1) {
    id=0; nextpiece=0; nprot=0; scale=0; npcol=1; mouseover=0; posX=0; posY=0; away=false; position=0;

    awayText.setFont(font2);
    awayText.setCharacterSize(48);
    awayText.setPosition(110, 150);
    positionText.setFont(font2);
    positionText.setCharacterSize(48);
    positionText.setPosition(130, 220);

    comboText.setFont(font2); comboText.setString("0");
    pendingText.setFont(font2); pendingText.setString("0");
    bpmText.setFont(font2); bpmText.setString("0");
    comboText.setCharacterSize(48);
    comboText.setColor(sf::Color::White);
    comboText.setPosition(360,270);
    pendingText.setCharacterSize(48);
    pendingText.setColor(sf::Color::White);
    pendingText.setPosition(360,500);
    bpmText.setCharacterSize(48);
    bpmText.setColor(sf::Color::White);
    bpmText.setPosition(360, 400);

    for (int x=0; x<4; x++)
        for (int y=0; y<4; y++)
            grid[y][x]=0;
}