#ifndef GAMEFIELD_H
#define GAMEFIELD_H

#include <SFML/Graphics.hpp>

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

    gameField(sf::Sprite* tilep, sf::Sprite* backgroundp);
    gameField(const gameField& field);

    void clear();

    void drawField();

    sf::Sprite* getSprite() { return &sprite; }

    void setsquare(short y, short x, short color) { square[y][x] = color; }

    short getsquare(short y, short x) { return square[y][x]; }

    void removeline(short y);

    sf::Vector2i clearlines();

    void setName(const sf::String& n, const sf::Font& font);
};

class obsField : public gameField {
public:
    obsField(const obsField& field);
    
    obsField(sf::Sprite* tilep, sf::Sprite* backgroundp);

    sf::Uint16 id;
    short nextpiece;
    short nprot;
    short npcol;
    short scale;
    short posX, posY;
    sf::Uint8 grid[4][4];
    bool mouseover;
    sf::Uint8 datacount;
    bool away;
    sf::Uint8 position;

    void drawField();
};

#endif