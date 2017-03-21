#include "network.h"
#include <SFML/Network.hpp>
#include <iostream>

void network::sendTCP() { while (tcpSock.send(packet) == sf::Socket::Partial) {} }
void network::sendUDP() { udpSock.send(packet, serverAdd, udpPort); }

sf::Socket::Status network::connect() {
	tcpSock.setBlocking(true);
	sf::Socket::Status status = tcpSock.connect(serverAdd, tcpPort, sf::seconds(5));
	tcpSock.setBlocking(false);
	return status;
}

bool network::receiveData() {
	sf::Socket::Status status;
	packet.clear();
	status = tcpSock.receive(packet);
	if (status == sf::Socket::Disconnected) {
		std::cout << "TCP disconnected" << std::endl;
		tcpSock.disconnect();
		packetid=255;
		return true;
	}
	if (status == sf::Socket::Done) {
		packet >> packetid;
		return true;
	}
	sf::IpAddress receiveAdd;
	unsigned short receivePort;
	packet.clear();
	status = udpSock.receive(packet, receiveAdd, receivePort);
	if (status == sf::Socket::Done) {
		packet >> packetid;
		return true;
	}
	else if (status != sf::Socket::NotReady)
		std::cout << "Udp not recieved: " << (int)status << std::endl;
	return false;
}