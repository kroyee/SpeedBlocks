#include "OnlineplayUI.h"
#include "gui.h"
#include "network.h"
using std::to_string;

void OnlineplayUI::create(sf::Rect<int> _pos, UI* _gui) {
	createBase(_pos, _gui);

	opTab = gui->themeBB->load("Tab");
	opTab->add("Rooms");
	opTab->add("Lobby");
	opTab->add("Tournaments");
	opTab->add("Create room");
	opTab->add("Back");
	opTab->setTabHeight(50);
	opTab->setPosition(50, 20);
	opTab->select(1);
	opTab->connect("TabSelected", &OnlineplayUI::opTabSelect, this);
	opTab->setOpacity(0.7);
	panel->add(opTab);

	sf::Rect<int> pos(0,100,450,500);
	roomList.create(pos, gui, panel);

	ServerLobby = tgui::Panel::create();
	ServerLobby->setPosition(0,100);
	ServerLobby->setSize(960, 500);
	ServerLobby->hide();
	ServerLobby->setBackgroundColor(sf::Color(255,255,255,0));
	panel->add(ServerLobby);

	Lobby = gui->themeBB->load("ChatBox");
	Lobby->setPosition(5, 5);
	Lobby->setSize(750, 455);
	ServerLobby->add(Lobby);

	ChatBox = gui->themeTG->load("EditBox");
	ChatBox->setPosition(5, 465);
	ChatBox->setSize(750, 30);
	ChatBox->connect("ReturnKeyPressed", &UI::sendMsg, gui, "Lobby");
	ChatBox->connect("Focused Unfocused", &UI::chatFocus, gui, std::bind(&tgui::Widget::isFocused, ChatBox));
	ServerLobby->add(ChatBox);

	LobbyList = gui->themeBB->load("ListBox");
	LobbyList->setPosition(760, 5);
	LobbyList->setSize(190, 490);
	LobbyList->getRenderer()->setTextColor(sf::Color::Black);
	ServerLobby->add(LobbyList);

	tournamentList.create(pos, gui, panel);

	CreateRoom = tgui::Panel::create();
	CreateRoom->setPosition(0,100);
	CreateRoom->setSize(960, 500);
	CreateRoom->hide();
	CreateRoom->setBackgroundColor(sf::Color(255,255,255,0));
	panel->add(CreateRoom);

	tgui::Label::Ptr NorL = gui->themeTG->load("Label");
	NorL->setPosition(30, 30);
	NorL->setText("Room name");
	CreateRoom->add(NorL);

	tgui::EditBox::Ptr NorE = gui->themeTG->load("EditBox");
	NorE->setPosition(25, 60);
	NorE->setSize(250, 40);
	NorE->connect("Focused Unfocused", &UI::chatFocus, gui, std::bind(&tgui::Widget::isFocused, NorE));
	CreateRoom->add(NorE);

	tgui::Label::Ptr NopL = gui->themeTG->load("Label");
	NopL->setPosition(430, 30);
	NopL->setText("Max players (0=unlimited)");
	CreateRoom->add(NopL);

	tgui::EditBox::Ptr NopE = gui->themeTG->load("EditBox");
	NopE->setPosition(425, 60);
	NopE->setSize(250, 40);
	NopE->setInputValidator(tgui::EditBox::Validator::UInt);
	NopE->connect("Focused Unfocused", &UI::chatFocus, gui, std::bind(&tgui::Widget::isFocused, NopE));
	CreateRoom->add(NopE);

	tgui::Button::Ptr CrB = gui->themeBB->load("Button");
	CrB->setPosition(300, 150);
	CrB->setText("Create");
	CrB->setSize(100, 40);
	CrB->connect("Pressed", &OnlineplayUI::createRoom, this, std::bind(&tgui::EditBox::getText, NorE), std::bind(&tgui::EditBox::getText, NopE));
	CreateRoom->add(CrB);
}

void OnlineplayUI::opTabSelect(const std::string& tab) {
	if (tab == "Rooms") {
		roomList.show();
		tournamentList.hide();
		ServerLobby->hide();
		CreateRoom->hide();
	}
	else if (tab == "Lobby") {
		ServerLobby->show();
		roomList.hide();
		tournamentList.hide();
		CreateRoom->hide();
		ChatBox->focus();
	}
	else if (tab == "Tournaments") {
		tournamentList.show();
		roomList.hide();
		ServerLobby->hide();
		CreateRoom->hide();
	}
	else if (tab == "Create room") {
		ServerLobby->hide();
		roomList.hide();
		tournamentList.hide();
		CreateRoom->show();
	}
	else if (tab == "Back") {
		hide();
		gui->mainMenu.show();
		gui->disconnect();
	}
}

void OnlineplayUI::createRoom(const sf::String& name, const sf::String& maxplayers) {
	if (!name.getSize())
		return;
	if (!maxplayers.getSize())
		return;
	gui->sendPacket11(name, stoi(maxplayers.toAnsiString()) );
	opTab->select(0);
}

void OnlineplayUI::makeRoomList() {
	sf::Uint8 roomCount;

	gui->net.packet >> roomCount;
	roomList.removeAllItems();

	for (int i=0; i<roomCount; i++)
		addRoom();
}

void OnlineplayUI::addRoom() {
	sf::String name;
	sf::Uint8 maxPlayers, currentPlayers;
	sf::Uint16 id;
	gui->net.packet >> id >> name >> currentPlayers >> maxPlayers;
	sf::String roomlabel = to_string(currentPlayers);
	if (maxPlayers)
		roomlabel += "/" + to_string(maxPlayers);
	roomlabel+= " players";
	roomList.addItem(name, roomlabel, id);
}

void OnlineplayUI::makeClientList() {
	clientInfo client;
	sf::Uint16 clientCount;

	gui->net.packet >> clientCount;

	for (int i=0; i<clientCount; i++) {
		gui->net.packet >> client.id >> client.name;
		clientList.push_back(client);
	}

	makeLobbyList();
}

void OnlineplayUI::makeLobbyList() {
	LobbyList->removeAllItems();
	for (auto&& client : clientList)
		LobbyList->addItem(client.name);
}

void OnlineplayUI::addClient() {
	clientInfo client;

	gui->net.packet >> client.id >> client.name;
	clientList.push_back(client);

	LobbyList->addItem(client.name);
}

void OnlineplayUI::removeClient() {
	sf::Uint16 id;

	gui->net.packet >> id;

	for (auto it = clientList.begin(); it != clientList.end(); it++)
		if (it->id == id) {
			LobbyList->removeItem(it->name);
			clientList.erase(it);
			break;
		}
}