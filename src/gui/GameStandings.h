#ifndef GAMESTANDINGS_H
#define GAMESTANDINGS_H

#include "GuiBase.h"

namespace sf {
class Packet;
}

class Resources;

class GameStandings : public GuiBase {
   public:
    os::Label rounds;
    os::Label p1_sets;
    os::Label p2_sets;
    os::Label p1_rounds;
    os::Label p2_rounds;

    uint16_t p1_id, p2_id;
    uint8_t _p1_sets, _p2_sets, _p1_rounds, _p2_rounds;

    GameStandings(sf::Rect<int> _pos, Resources& _res);

    void setResults(sf::Packet& packet);
    void setWaitTime(uint16_t time);
    void alignResult();
};

#endif