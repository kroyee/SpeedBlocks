#ifndef PINGHANDLE_H
#define PINGHANDLE_H

#include <SFML/System.hpp>
#include <deque>

namespace sf {
class Packet;
}

struct NP_Ping;

class PingHandle {
   private:
    struct PingPacket {
        uint8_t id;
        sf::Time sent, received;
        uint16_t ping;
        bool returned = false;
    };

    std::deque<PingPacket> packets;
    uint8_t pingIdCount = 0, packetCount = 0;
    sf::Time lastSend = sf::seconds(0);

   public:
    int get(const sf::Time& t, const NP_Ping&);
    int send(const sf::Time& t, uint16_t myId);
    int getAverage();
    float getPacketLoss(const sf::Time& t);
    int getLowest();
};

#endif