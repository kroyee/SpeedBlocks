#ifndef GAMEDATASENDER_H
#define GAMEDATASENDER_H

#include <SFML/Network.hpp>
class GamePlay;

class GameDataSender {
    GamePlay& game;
    sf::Time sendTime;
    uint8_t count;

   public:
    GameDataSender(GamePlay&);

    void sendstate();
    void state();
    void gameover();
    void reset();
};

#endif