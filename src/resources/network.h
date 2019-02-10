#ifndef NETWORK_H
#define NETWORK_H

#include <curl/curl.h>
#include <stdio.h>
#include <SFML/Network.hpp>
#include <list>

class network {
   public:
    network();
    ~network();

    sf::TcpSocket tcpSock;
    sf::UdpSocket udpSock;

    sf::IpAddress serverAdd;
    unsigned short tcpPort;
    unsigned short udpPort;

    sf::Socket::Status connect();
    void disconnect(int) { tcpSock.disconnect(); }
    void sendTCP(sf::Packet&);
    void sendUDP(sf::Packet&);
    void sendSignal(int signalID, int val1 = -1, int val2 = -1);
    void sendUdpConfirm(uint16_t);
    void sendPing(int myID, int pingID);

    std::string sendCurlPost(const std::string& URL, const std::string& postData, uint8_t type);

    void receiveData();
    void sendData();
};

#endif