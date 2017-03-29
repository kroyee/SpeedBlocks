#ifndef NETWORK_H
#define NETWORK_H

#include <SFML/Network.hpp>
#include <list>

class network {
public:
	network() { tcpSock.setBlocking(false); udpSock.setBlocking(false); serverAdd="darn.site"; tcpPort=21512; udpPort=21514; }

	sf::TcpSocket tcpSock;
	sf::UdpSocket udpSock;

	sf::IpAddress serverAdd;
	unsigned short tcpPort;
	unsigned short udpPort;
	unsigned short localUdpPort;

	sf::Packet packet;
	sf::Uint8 packetid;

	sf::Socket::Status connect();
	void disconnect() { udpSock.unbind(); tcpSock.disconnect(); }
	void sendTCP();
	void sendUDP();

	bool receiveData();
};

#endif