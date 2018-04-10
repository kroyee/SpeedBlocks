#include "network.h"
#include "GameSignals.h"
#include <SFML/Network.hpp>
#include <iostream>
#include <cstring>
using std::cout;
using std::endl;

network::network() : serverAdd("localhost"), tcpPort(21512), udpPort(21514) {
	tcpSock.setBlocking(false);
	udpSock.setBlocking(false);
	udpSock.bind(sf::Socket::AnyPort);
	curl_global_init(CURL_GLOBAL_ALL);

	connectSignal("SendSignal", &network::sendSignal, this);
	connectSignal("SendPacket", &network::sendTCP, this);
	connectSignal("SendPacketUDP", &network::sendUDP, this);
	connectSignal("SendPing", &network::sendPing, this);
	connectSignal("Disconnect", &network::disconnect, this);
}

network::~network() {
	curl_global_cleanup();
}

void network::sendTCP(sf::Packet& packet) { while (tcpSock.send(packet) == sf::Socket::Partial) {} }
void network::sendUDP(sf::Packet& packet) { udpSock.send(packet, serverAdd, udpPort); }

void network::sendSignal(int signalID, int val1, int val2) {
	sf::Packet packet;
	packet << (uint8_t)254 << static_cast<uint8_t>(signalID);
	if (val1 > -1)
		packet << static_cast<uint16_t>(val1);
	if (val2 > -1)
		packet << static_cast<uint16_t>(val2);
	sendTCP(packet);
}

void network::sendUdpConfirm(uint16_t id) {
	sf::Packet packet;
	packet << (uint8_t)99 << id;
	sendUDP(packet);
}

void network::sendPing(int myID, int pingID) {
	sf::Packet packet;
	packet << (uint8_t)102 << static_cast<uint16_t>(myID) << static_cast<uint8_t>(pingID);
	sendUDP(packet);
}

struct MemoryStruct {
  char *memory;
  size_t size;
};
 
static size_t
WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
  size_t realsize = size * nmemb;
  struct MemoryStruct *mem = (struct MemoryStruct *)userp;
 
  mem->memory = (char*)realloc(mem->memory, mem->size + realsize + 1);
  if(mem->memory == NULL) {
    /* out of memory! */ 
    cout << "not enough memory (realloc returned NULL)" << endl;
    return 0;
  }
 
  memcpy(&(mem->memory[mem->size]), contents, realsize);
  mem->size += realsize;
  mem->memory[mem->size] = 0;
 
  return realsize;
}

std::string network::sendCurlPost(const std::string& URL, const std::string& postData, uint8_t type) {
	CURL *curl;
	CURLcode res;

	struct MemoryStruct chunk;
 
  	chunk.memory = (char*)malloc(1);  /* will be grown as needed by the realloc above */ 
  	chunk.size = 0;    /* no data at this point */ 

  	char * cstr = new char [postData.size()+1];
	std::strcpy (cstr, postData.c_str());

	char * urlstr = new char [URL.size()+1];
	std::strcpy (urlstr, URL.c_str());

	std::string response;

	curl = curl_easy_init();
	if(curl) {
		curl_easy_setopt(curl, CURLOPT_URL, urlstr);

		struct curl_slist *headers = NULL;
		if (type == 0) {
			headers = curl_slist_append(headers, "Content-Type: application/json");
			//curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);
		}
		else
			headers = curl_slist_append(headers, "Content-Type: application/x-www-form-urlencoded");
		headers = curl_slist_append(headers, "Cache-Control: no-cache");
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

		/* Now specify the POST data */ 
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, cstr);

		// Specify callbackfunction to get the response
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
  		curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);

		/* Perform the request, res will get the return code */ 
		res = curl_easy_perform(curl);
		/* Check for errors */ 
		if(res != CURLE_OK) {
			fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
			cout << endl;
		}

		curl_slist_free_all(headers);

		/* always cleanup */ 
		curl_easy_cleanup(curl);
	}
	else
		cout << "Curl failed to load" << endl;

	delete[] cstr;
	delete[] urlstr;

	if (res == CURLE_OK) {
		std::string response(chunk.memory);
		free(chunk.memory);
		return response;
	}
	else
		return "Error";
}

sf::Socket::Status network::connect() {
	tcpSock.setBlocking(true);
	sf::Socket::Status status = tcpSock.connect(serverAdd, tcpPort, sf::seconds(5));
	tcpSock.setBlocking(false);
	return status;
}

void getSignal(sf::Packet &packet) {
	uint8_t signalId;
	uint16_t id1, id2;

	packet >> signalId;
	if (!packet.endOfPacket()) {
			packet >> id1;
		if (!packet.endOfPacket()) {
			packet >> id2;
			if (!Net::passOnSignal(signalId, id1, id2))
				cout << "Error passing on signal " << signalId << "(x,y)" << endl;
			return;
		}
		if (!Net::passOnSignal(signalId, id1))
			cout << "Error passing on signal " << signalId << "(x)" << endl;
		return;
	}
	if (!Net::passOnSignal(signalId))
		cout << "Error passing on signal " << signalId << "()" << endl;
}

static auto& Disconnect = Signal<void, int>::get("Disconnect");

bool network::receiveData() {
	sf::Packet packet;
	sf::Socket::Status status = tcpSock.receive(packet);
	uint8_t packetid;
	if (status == sf::Socket::Disconnected) {
		cout << "TCP disconnected" << endl;
		Disconnect(1);
		return true;
	}
	if (status == sf::Socket::Done) {
		packet >> packetid;
		if (packetid < 100)
			cout << "Packet: " << static_cast<int>(packetid) << endl;
		if (packetid == 254) {
			getSignal(packet);
			return true;
		}
		Net::passOnPacket(packetid, packet);
		return true;
	}
	sf::IpAddress receiveAdd;
	unsigned short receivePort;
	status = udpSock.receive(packet, receiveAdd, receivePort);
	if (status == sf::Socket::Done) {
		packet >> packetid;
		Net::passOnPacket(packetid, packet);
		return true;
	}
	return false;
}