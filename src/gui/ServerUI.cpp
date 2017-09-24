#include "ServerUI.h"
#include "Signal.h"
#include <SFML/Network.hpp>

ServerUI::ServerUI(sf::Rect<int> _pos, Resources& _res, tgui::Panel::Ptr parent) : guiBase(_pos, _res, parent) {

	tgui::Label::Ptr plist = resources.gfx->themeTG->load("Label");
	plist->setPosition(410, 20);
	plist->setText("Player list");
	panel->add(plist);

	LobbyList = resources.gfx->themeTG->load("ListBox");
	LobbyList->setPosition(360, 50);
	LobbyList->setSize(190, 490);
	panel->add(LobbyList);

	tgui::Label::Ptr header = resources.gfx->themeTG->load("Label");
	header->setText("Message of the Day");
	header->setTextSize(24);
	header->setPosition(60, 10);
	panel->add(header);

	motd = resources.gfx->themeTG->load("Label");
	motd->setPosition(10, 50);
	motd->setMaximumTextWidth(340);
	panel->add(motd);

	tgui::Label::Ptr joinUs = resources.gfx->themeTG->load("Label");
	joinUs->setText("Join speedblocks on");
	joinUs->setPosition(100, 390);
	panel->add(joinUs);

	tgui::Button::Ptr discord = resources.gfx->themeTG->load("Button");
	discord->setPosition(10, 430);
	discord->setSize(100, 30);
	discord->setText("Discord");
	discord->connect("pressed", &ServerUI::linkPressed, this, 1);
	panel->add(discord);

	tgui::Button::Ptr github = resources.gfx->themeTG->load("Button");
	github->setPosition(130, 430);
	github->setSize(100, 30);
	github->setText("GitHub");
	github->connect("pressed", &ServerUI::linkPressed, this, 2);
	panel->add(github);

	tgui::Button::Ptr webpage = resources.gfx->themeTG->load("Button");
	webpage->setPosition(250, 430);
	webpage->setSize(100, 30);
	webpage->setText("WebPage");
	webpage->connect("pressed", &ServerUI::linkPressed, this, 3);
	panel->add(webpage);

	tgui::Button::Ptr forum = resources.gfx->themeTG->load("Button");
	forum->setPosition(10, 480);
	forum->setSize(100, 30);
	forum->setText("Forum");
	forum->connect("pressed", &ServerUI::linkPressed, this, 4);
	panel->add(forum);

	tgui::Button::Ptr stats = resources.gfx->themeTG->load("Button");
	stats->setPosition(130, 480);
	stats->setSize(100, 30);
	stats->setText("Player stats");
	stats->connect("pressed", &ServerUI::linkPressed, this, 5);
	panel->add(stats);

	Net::takePacket(20, &ServerUI::addClient, this);
	Net::takePacket(21, &ServerUI::removeClient, this);
}

void ServerUI::makeClientList(sf::Packet &packet) {
	LobbyList->removeAllItems();
	resources.clientList.clear();
	clientInfo client;
	sf::Uint16 clientCount;

	packet >> clientCount;

	for (int i=0; i<clientCount; i++) {
		packet >> client.id >> client.name;
		resources.clientList.push_back(client);
	}

	makeLobbyList();
}

void ServerUI::makeLobbyList() {
	LobbyList->removeAllItems();
	for (auto&& client : resources.clientList)
		LobbyList->addItem(client.name);
}

void ServerUI::addClient(sf::Packet &packet) {
	clientInfo client;

	packet >> client.id >> client.name;
	resources.clientList.push_back(client);

	LobbyList->addItem(client.name);
}

void ServerUI::putClient(sf::Uint16 id, const sf::String& name) {
	clientInfo client;
	client.id = id;
	client.name = name;
	resources.clientList.push_back(client);
	LobbyList->addItem(client.name);
}

void ServerUI::removeClient(sf::Packet &packet) {
	sf::Uint16 id;

	packet >> id;

	for (auto it = resources.clientList.begin(); it != resources.clientList.end(); it++)
		if (it->id == id) {
			LobbyList->removeItem(it->name);
			resources.clientList.erase(it);
			break;
		}
}

void ServerUI::linkPressed(sf::Uint8 type) {
	std::string url;
	#ifdef _WIN32
		url = "start ";
	#elif __APPLE__
		url = "open ";
	#else
		url = "xdg-open ";
	#endif
	if (type == 1) //Discord
		url += "https://discord.gg/3qZKvaj";
	if (type == 2) //GitHub
		url += "https://github.com/kroyee/SpeedBlocks";
	if (type == 3) //WebPage
		url += "https://speedblocks.se/";
	if (type == 4) //Forum
		url += "https://speedblocks.se/forum/";
	if (type == 5) //Stats
		url += "https://speedblocks.se/forum/stats.php";

	system(url.c_str());
}

void ServerUI::clear() {
	LobbyList->removeAllItems();
	resources.clientList.clear();
}