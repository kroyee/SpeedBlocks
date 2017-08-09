#include "ServerUI.h"
#include "gui.h"

void ServerUI::create(sf::Rect<int> _pos, UI* _gui, tgui::Panel::Ptr parent) {
	createBase(_pos, _gui, parent);

	tgui::Label::Ptr plist = gui->themeTG->load("Label");
	plist->setPosition(410, 20);
	plist->setText("Player list");
	panel->add(plist);

	LobbyList = gui->themeTG->load("ListBox");
	LobbyList->setPosition(360, 50);
	LobbyList->setSize(190, 490);
	panel->add(LobbyList);

	tgui::Label::Ptr header = gui->themeTG->load("Label");
	header->setText("Message of the Day");
	header->setTextSize(24);
	header->setPosition(60, 10);
	panel->add(header);

	motd = gui->themeTG->load("Label");
	motd->setPosition(10, 50);
	motd->setMaximumTextWidth(340);
	panel->add(motd);

	tgui::Label::Ptr joinUs = gui->themeTG->load("Label");
	joinUs->setText("Join speedblocks on");
	joinUs->setPosition(100, 390);
	panel->add(joinUs);

	tgui::Button::Ptr discord = gui->themeTG->load("Button");
	discord->setPosition(10, 430);
	discord->setSize(100, 30);
	discord->setText("Discord");
	discord->connect("pressed", &ServerUI::linkPressed, this, 1);
	panel->add(discord);

	tgui::Button::Ptr github = gui->themeTG->load("Button");
	github->setPosition(130, 430);
	github->setSize(100, 30);
	github->setText("GitHub");
	github->connect("pressed", &ServerUI::linkPressed, this, 2);
	panel->add(github);

	tgui::Button::Ptr webpage = gui->themeTG->load("Button");
	webpage->setPosition(250, 430);
	webpage->setSize(100, 30);
	webpage->setText("WebPage");
	webpage->connect("pressed", &ServerUI::linkPressed, this, 3);
	panel->add(webpage);

	tgui::Button::Ptr forum = gui->themeTG->load("Button");
	forum->setPosition(10, 480);
	forum->setSize(100, 30);
	forum->setText("Forum");
	forum->connect("pressed", &ServerUI::linkPressed, this, 4);
	panel->add(forum);

	tgui::Button::Ptr stats = gui->themeTG->load("Button");
	stats->setPosition(130, 480);
	stats->setSize(100, 30);
	stats->setText("Player stats");
	stats->connect("pressed", &ServerUI::linkPressed, this, 5);
	panel->add(stats);
}

void ServerUI::makeClientList() {
	LobbyList->removeAllItems();
	clientList.clear();
	clientInfo client;
	sf::Uint16 clientCount;

	gui->net.packet >> clientCount;

	for (int i=0; i<clientCount; i++) {
		gui->net.packet >> client.id >> client.name;
		clientList.push_back(client);
	}

	makeLobbyList();
}

void ServerUI::makeLobbyList() {
	LobbyList->removeAllItems();
	for (auto&& client : clientList)
		LobbyList->addItem(client.name);
}

void ServerUI::addClient() {
	clientInfo client;

	gui->net.packet >> client.id >> client.name;
	clientList.push_back(client);

	LobbyList->addItem(client.name);
}

void ServerUI::putClient(sf::Uint16 id, const sf::String& name) {
	clientInfo client;
	client.id = id;
	client.name = name;
	clientList.push_back(client);
	LobbyList->addItem(client.name);
}

void ServerUI::removeClient() {
	sf::Uint16 id;

	gui->net.packet >> id;

	for (auto it = clientList.begin(); it != clientList.end(); it++)
		if (it->id == id) {
			LobbyList->removeItem(it->name);
			clientList.erase(it);
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
	clientList.clear();
}