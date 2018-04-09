#include "PingHandle.h"
#include "GameSignals.h"
#include <SFML/Network.hpp>
#include <iostream>

static auto& SendPacketUDP = Signal<void, sf::Packet&>::get("SendPacketUDP");
static auto& SendPing = Signal<void, int, int>::get("SendPing");

int PingHandle::get(const sf::Time& t, sf::Packet &_packet) {
	uint8_t pingId;
	uint16_t clientid;
	_packet >> clientid >> pingId;

	for (auto& packet : packets) {
		if (packet.id == pingId) {
			SendPacketUDP(_packet);
			packet.received = t;
			packet.ping = (packet.received-packet.sent).asMilliseconds();
			packet.returned = true;
			return packet.ping;
		}
		else if (packet.returned)
			break;
	}
	return -1;
}

int PingHandle::send(const sf::Time& t, uint16_t myId) {
	if (t < lastSend)
		return -1;
	int ping=-1;
	for (auto& packet : packets) {
		if (!packet.returned)
			ping+=300;
		else break;
	}
	SendPing(myId, pingIdCount);
	lastSend=t+sf::milliseconds(300);
	PingPacket packet;
	packet.id = pingIdCount++;
	packet.sent = t;
	packets.push_front(packet);
	packetCount++;
	if (packetCount > 16) {
		packets.pop_back();
		packetCount--;
	}
	return ping;
}

int PingHandle::getAverage() {
	int count, totalPing;
	for (auto& packet : packets)
		if (packet.returned) {
			count++;
			totalPing+=packet.ping;
		}
	return totalPing/count;
}

float PingHandle::getPacketLoss(const sf::Time& t) {
	bool ret=false;
	int total, dropped;
	for (auto& packet : packets) {
		if (!ret) {
			if (packet.returned)
				ret=true;
			else if (t - packet.sent > sf::milliseconds(1000))
				ret=true;
		}
		if (ret) {
			if (packet.returned)
				total++;
			else {
				total++;
				dropped++;
			}
		}
	}
	return (float)dropped/(float)total;
}

int PingHandle::getLowest() {
	int lowest=100000;
	for (auto& packet : packets)
		if (packet.returned)
			if (packet.ping < lowest)
				lowest=packet.ping;
	return lowest;
}