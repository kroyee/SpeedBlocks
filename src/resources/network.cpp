#include "network.h"
#include <SFML/Network.hpp>
#include <iostream>
#include <cstring>
using std::cout;
using std::endl;

network::network() : serverAdd("localhost"), tcpPort(21512), udpPort(21514) {
	tcpSock.setBlocking(false);
	udpSock.setBlocking(false);
	curl_global_init(CURL_GLOBAL_ALL);
}

network::~network() {
	curl_global_cleanup();
}

void network::sendTCP() { while (tcpSock.send(packet) == sf::Socket::Partial) {} }
void network::sendUDP() { udpSock.send(packet, serverAdd, udpPort); }

void network::sendSignal(sf::Uint8 signalId, sf::Uint16 id1, sf::Uint16 id2) {
	packet.clear();
	packet << (sf::Uint8)254 << signalId;
	if (id1)
		packet << id1;
	if (id2)
		packet << id2;
	sendTCP();
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

sf::String network::sendCurlPost(const sf::String& URL, const sf::String& postData, sf::Uint8 type) {
	CURL *curl;
	CURLcode res;

	struct MemoryStruct chunk;
 
  	chunk.memory = (char*)malloc(1);  /* will be grown as needed by the realloc above */ 
  	chunk.size = 0;    /* no data at this point */ 

  	char * cstr = new char [postData.getSize()+1];
	std::strcpy (cstr, postData.toAnsiString().c_str());

	char * urlstr = new char [URL.getSize()+1];
	std::strcpy (urlstr, URL.toAnsiString().c_str());

	sf::String response;

	curl = curl_easy_init();
	if(curl) {
		curl_easy_setopt(curl, CURLOPT_URL, urlstr);

		struct curl_slist *headers = NULL;
		if (type == 0) {
			headers = curl_slist_append(headers, "Content-Type: application/json");
			curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);
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
		sf::String response(chunk.memory);
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

bool network::receiveData() {
	sf::Socket::Status status;
	packet.clear();
	status = tcpSock.receive(packet);
	if (status == sf::Socket::Disconnected) {
		cout << "TCP disconnected" << endl;
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
		cout << "Udp not recieved: " << (int)status << endl;
	return false;
}