#include "ServerUI.h"
#include <SFML/Network.hpp>
#include "GameSignals.h"
#include "Resources.h"

ServerUI::ServerUI(sf::Rect<int> _pos, Resources& _res, os::Panel& parent) : GuiBase(_pos, _res, parent) {
    os::Label().pos(410, 20).text("Player list").add_to(panel);

    LobbyList.pos(360, 50).size(190, 490).add_to(panel);

    os::Label().text("Message of the Day").text_size(24).pos(60, 10).add_to(panel);

    motd.pos(10, 50).max(340).add_to(panel);

    os::Label().text("Join speedblocks on").pos(100, 390).add_to(panel);

    os::Button().pos(10, 430).size(100, 30).text("Discord").connect("pressed", &ServerUI::linkPressed, this, 1).add_to(panel);

    os::Button().pos(130, 430).size(100, 30).text("GitHub").connect("pressed", &ServerUI::linkPressed, this, 2).add_to(panel);

    os::Button().pos(250, 430).size(100, 30).text("WebPage").connect("pressed", &ServerUI::linkPressed, this, 3).add_to(panel);

    os::Button().pos(10, 480).size(100, 30).text("Forum").connect("pressed", &ServerUI::linkPressed, this, 4).add_to(panel);

    os::Button().pos(130, 480).size(100, 30).text("Player stats").connect("pressed", &ServerUI::linkPressed, this, 5).add_to(panel);

    Net::takePacket(20, &ServerUI::addClient, this);
    Net::takePacket(21, &ServerUI::removeClient, this);
}
#include <iostream>
void ServerUI::makeClientList(sf::Packet& packet) {
    LobbyList->removeAllItems();
    resources.clientList.clear();
    clientInfo client;
    uint16_t clientCount;

    packet >> clientCount;

    for (int i = 0; i < clientCount; i++) {
        packet >> client.id >> client.name;
        resources.clientList.push_back(client);
    }

    makeLobbyList();
}

void ServerUI::makeLobbyList() {
    LobbyList.clear();
    for (auto&& client : resources.clientList) LobbyList.add(client.name);
}

void ServerUI::addClient(sf::Packet& packet) {
    clientInfo client;

    packet >> client.id >> client.name;
    resources.clientList.push_back(client);

    LobbyList.add(client.name);
}

void ServerUI::putClient(uint16_t id, const std::string& name) {
    clientInfo client;
    client.id = id;
    client.name = name;
    resources.clientList.push_back(client);
    LobbyList.add(client.name);
}

void ServerUI::removeClient(sf::Packet& packet) {
    uint16_t id;

    packet >> id;

    for (auto it = resources.clientList.begin(); it != resources.clientList.end(); it++)
        if (it->id == id) {
            LobbyList.remove(it->name);
            resources.clientList.erase(it);
            break;
        }
}

void ServerUI::linkPressed(uint8_t type) {
    std::string url;
#ifdef _WIN32
    url = "start ";
#elif __APPLE__
    url = "open ";
#else
    url = "xdg-open ";
#endif
    if (type == 1)  // Discord
        url += "https://discord.gg/3qZKvaj";
    if (type == 2)  // GitHub
        url += "https://github.com/kroyee/SpeedBlocks";
    if (type == 3)  // WebPage
        url += "https://speedblocks.se/";
    if (type == 4)  // Forum
        url += "https://speedblocks.se/forum/";
    if (type == 5)  // Stats
        url += "https://speedblocks.se/forum/stats.php";

    system(url.c_str());
}

void ServerUI::clear() {
    LobbyList.clear();
    resources.clientList.clear();
}
