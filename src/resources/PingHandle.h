#ifndef PINGHANDLE_H
#define PINGHANDLE_H

#include <SFML/System.hpp>
#include <deque>

namespace sf { class Packet; }

class PingHandle {
private:
	struct PingPacket {
		sf::Uint8 id;
		sf::Time sent, received;
		sf::Uint16 ping;
		bool returned=false;
	};

	std::deque<PingPacket> packets;
	sf::Uint8 pingIdCount=0, packetCount=0;
	sf::Time lastSend=sf::seconds(0);

public:
	int get(const sf::Time& t, sf::Packet &_packet);
	int send(const sf::Time& t, sf::Uint16 myId);
	int getAverage();
	float getPacketLoss(const sf::Time& t);
	int getLowest();
};

#endif