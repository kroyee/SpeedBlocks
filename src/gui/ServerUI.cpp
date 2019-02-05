#include "ServerUI.h"
#include <SFML/Network.hpp>
#include "GameSignals.h"
#include "Packets.hpp"
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

    PM::handle_packet([&](const NP_ClientJoinedServer& p) {
        resources.clientList.push_back({p.player.id, p.player.name});
        LobbyList.add(p.player.name);
    });

    PM::handle_packet([&](const NP_ClientLeftServer& p) {
        for (auto it = resources.clientList.begin(); it != resources.clientList.end(); it++) {
            if (it->id == p.id) {
                LobbyList.remove(it->name);
                resources.clientList.erase(it);
                return;
            }
        }
    });

    PM::handle_packet([&](const NP_ClientList& p) {
        LobbyList->removeAllItems();
        resources.clientList.clear();

        for (auto& client : p.clients) {
            resources.clientList.push_back({client.id, client.name});
        }

        makeLobbyList();
    });
}

void ServerUI::makeLobbyList() {
    LobbyList.clear();
    for (auto&& client : resources.clientList) LobbyList.add(client.name);
}

void ServerUI::putClient(uint16_t id, const std::string& name) {
    clientInfo client;
    client.id = id;
    client.name = name;
    resources.clientList.push_back(client);
    LobbyList.add(client.name);
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
