#include "OnlineplayUI.h"
#include "gui.h"
#include "network.h"
#include "MainMenu.h"
#include "gamePlay.h"
using std::to_string;
using std::cout;
using std::endl;

void OnlineplayUI::create(sf::Rect<int> _pos, UI* _gui) {
	createBase(_pos, _gui);

	opTab = gui->themeBB->load("Tab");
	opTab->add("Rooms");
	opTab->add("Lobby");
	opTab->add("Tournaments");
	opTab->add("Challenges");
	opTab->add("Back");
	opTab->setTabHeight(50);
	opTab->setPosition(50, 20);
	opTab->select(1);
	opTab->connect("TabSelected", &OnlineplayUI::opTabSelect, this);
	opTab->setOpacity(0.7);
	panel->add(opTab);

	sf::Rect<int> pos(0,100,450,500);
	roomList.create(pos, gui, panel);

	roomSidePanel = tgui::Panel::create();
	roomSidePanel->setPosition(460,100);
	roomSidePanel->setSize(400,500);
	roomSidePanel->setBackgroundColor(sf::Color(255,255,255,0));
	roomSidePanel->hide();
	panel->add(roomSidePanel);

	tgui::Button::Ptr widget8 = gui->themeTG->load("Button");
	widget8->setPosition(20,20);
	widget8->setSize(320,75);
	widget8->setText("Create Room");
	widget8->connect("pressed", &OnlineplayUI::createRoomPressed, this);
	roomSidePanel->add(widget8);

	matchButton = gui->themeTG->load("Button");
	matchButton->setPosition(20, 200);
	matchButton->setSize(320, 75);
	matchButton->setText("Join 1vs1 matchmaking");
	matchButton->connect("pressed", &OnlineplayUI::matchmakingPressed, this);
	roomSidePanel->add(matchButton);

	matchQueueing = gui->themeTG->load("Label");
	matchQueueing->setPosition(50,290);
	matchQueueing->setText("In queue: 0");
	matchQueueing->setTextSize(18);
	roomSidePanel->add(matchQueueing);

	matchPlaying = gui->themeTG->load("Label");
	matchPlaying->setPosition(220,290);
	matchPlaying->setText("Playing: 0");
	matchPlaying->setTextSize(18);
	roomSidePanel->add(matchPlaying);

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
	ChatBox->connect("ReturnKeyPressed", &UI::chatMsg, gui, "Lobby");
	ChatBox->connect("Focused Unfocused", &UI::chatFocus, gui, std::bind(&tgui::Widget::isFocused, ChatBox));
	ServerLobby->add(ChatBox);

	LobbyList = gui->themeBB->load("ListBox");
	LobbyList->setPosition(760, 5);
	LobbyList->setSize(190, 490);
	LobbyList->getRenderer()->setTextColor(sf::Color::Black);
	ServerLobby->add(LobbyList);

	tournamentList.create(pos, gui, panel);
	pos.width = 960;
	tournamentPanel.create(pos, gui, panel);

	tournamentSidePanel = tgui::Panel::create();
	tournamentSidePanel->setPosition(460,100);
	tournamentSidePanel->setSize(400,500);
	tournamentSidePanel->setBackgroundColor(sf::Color(255,255,255,0));
	tournamentSidePanel->hide();
	panel->add(tournamentSidePanel);

	tgui::Button::Ptr widget0 = gui->themeTG->load("Button");
	widget0->setPosition(20,20);
	widget0->setSize(320,75);
	widget0->setText("Create Tournament");
	widget0->connect("pressed", &OnlineplayUI::createTournamentPressed, this);
	tournamentSidePanel->add(widget0);

	createTournamentPanel = tgui::Panel::create();
	createTournamentPanel->setPosition(0,100);
	createTournamentPanel->setSize(960,500);
	createTournamentPanel->setBackgroundColor(sf::Color(255,255,255,0));
	createTournamentPanel->hide();
	panel->add(createTournamentPanel);

	tgui::Label::Ptr widget2 = gui->themeTG->load("Label");
	widget2->setPosition(70,40);
	widget2->setText("Tournament name");
	widget2->setTextSize(18);
	createTournamentPanel->add(widget2);

	tgui::Label::Ptr widget3 = gui->themeTG->load("Label");
	widget3->setPosition(87,100);
	widget3->setText("Sets");
	widget3->setTextSize(18);
	createTournamentPanel->add(widget3);

	tgui::Label::Ptr widget4 = gui->themeTG->load("Label");
	widget4->setPosition(340,100);
	widget4->setText("Rounds");
	widget4->setTextSize(18);
	createTournamentPanel->add(widget4);

	tournamentName = gui->themeTG->load("EditBox");
	tournamentName->setPosition(255,37);
	tournamentName->setSize(260,30);
	createTournamentPanel->add(tournamentName);

	sets = gui->themeTG->load("EditBox");
	sets->setPosition(140,97);
	sets->setSize(90,30);
	sets->setInputValidator(tgui::EditBox::Validator::UInt);
	createTournamentPanel->add(sets);

	rounds = gui->themeTG->load("EditBox");
	rounds->setPosition(420,97);
	rounds->setSize(90,30);
	rounds->setInputValidator(tgui::EditBox::Validator::UInt);
	createTournamentPanel->add(rounds);

	tgui::Button::Ptr widget6 = gui->themeTG->load("Button");
	widget6->setPosition(229,173);
	widget6->setSize(140,40);
	widget6->setText("Create");
	widget6->connect("pressed", &OnlineplayUI::createTournament, this);
	createTournamentPanel->add(widget6);

	tgui::Button::Ptr widget7 = gui->themeTG->load("Button");
	widget7->setPosition(400,173);
	widget7->setSize(140,40);
	widget7->setText("Back");
	widget7->connect("pressed", &OnlineplayUI::back, this);
	createTournamentPanel->add(widget7);

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

	challengesUI.create(pos, gui, panel);
}

void OnlineplayUI::opTabSelect(const std::string& tab) {
	if (tab == "Rooms") {
		hideAllPanels();
		roomList.show();
		roomSidePanel->show();
	}
	else if (tab == "Lobby") {
		hideAllPanels();
		ServerLobby->show();
		ChatBox->focus();
	}
	else if (tab == "Tournaments") {
		hideAllPanels();
		tournamentList.show();
		tournamentSidePanel->show();
	}
	else if (tab == "Create room") {
		hideAllPanels();
		CreateRoom->show();
	}
	else if (tab == "Challenges") {
		hideAllPanels();
		challengesUI.show();
	}
	else if (tab == "Back") {
		hide();
		gui->mainMenu->show();
		gui->disconnect();
	}
}

void OnlineplayUI::hideAllPanels(bool keepTournamentOpen) {
	ServerLobby->hide();
	roomList.hide();
	tournamentList.hide();
	CreateRoom->hide();
	if (!keepTournamentOpen)
		tournamentPanel.hide();
	tournamentSidePanel->hide();
	createTournamentPanel->hide();
	challengesUI.hide();
	roomSidePanel->hide();
}

void OnlineplayUI::createRoom(const sf::String& name, const sf::String& maxplayers) {
	if (!name.getSize())
		return;
	if (!maxplayers.getSize())
		return;
	sf::Uint8 packetid = 11;
	gui->net.packet.clear();
	gui->net.packet << packetid << name << (sf::Uint8)stoi(maxplayers.toAnsiString());
	gui->net.sendTCP();
	hideAllPanels();
	roomList.show();
	roomSidePanel->show();
}

void OnlineplayUI::makeRoomList() {
	sf::Uint8 roomCount;

	gui->net.packet >> roomCount;
	auto scrollpos = roomList.scroll->getValue();
	roomList.removeAllItems();

	for (int i=0; i<roomCount; i++)
		addRoom();

	roomList.scroll->setValue(scrollpos);

	sf::Uint16 inqueue, inplay;
	gui->net.packet >> inqueue >> inplay;
	matchQueueing->setText("In queue: " + to_string(inqueue));
	matchPlaying->setText("Playing: " + to_string(inplay));
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

void OnlineplayUI::makeTournamentList() {
	sf::Uint8 tournamentCount;

	gui->net.packet >> tournamentCount;
	auto scrollpos = tournamentList.scroll->getValue();
	tournamentList.removeAllItems();

	for (int i=0; i<tournamentCount; i++)
		addTournament();

	tournamentList.scroll->setValue(scrollpos);
}

void OnlineplayUI::addTournament() {
	sf::String name;
	sf::Uint8 status;
	sf::Uint16 id, players;
	gui->net.packet >> id >> name >> status >> players;
	sf::String label;
	if (status == 0)
		label = "Sign Up - ";
	else if (status == 1)
		label = "Pending - ";
	else if (status == 2)
		label = "Started - ";
	else if (status == 3)
		label = "Finished - ";
	else if (status == 4)
		label = "Aborted - ";
	else
		label = "? - ";
	label += to_string(players) + " players";
	tournamentList.addItem(name, label, id);
}

void OnlineplayUI::createRoomPressed() {
	hideAllPanels();
	CreateRoom->show();
}

void OnlineplayUI::createTournamentPressed() {
	hideAllPanels();
	createTournamentPanel->show();
}

void OnlineplayUI::matchmakingPressed() {
	if (matchButton->getText() == "Join 1vs1 matchmaking")
		gui->net.sendSignal(21);
	else
		gui->net.sendSignal(22);
}

void OnlineplayUI::back() {
	if (createTournamentPanel->isVisible()) {
		hideAllPanels();
		tournamentList.show();
		tournamentSidePanel->show();
	}
}

void OnlineplayUI::createTournament() {
	if (!tournamentName->getText().getSize() || !sets->getText().getSize() || !rounds->getText().getSize())
		return;
	sf::Uint8 setcount = stoi(sets->getText().toAnsiString());
	sf::Uint8 roundcount = stoi(rounds->getText().toAnsiString());
	gui->net.packet.clear();
	gui->net.packet << (sf::Uint8)21 << tournamentName->getText() << setcount << roundcount;
	gui->net.sendTCP();
	back();
	updateTournamentListTime -= sf::seconds(5);
}

void OnlineplayUI::alertMsg(const sf::Uint16 id1) {
	sf::String msg = "Tournament game ready";
	for (auto&& tournament : tournamentList.items)
		if (tournament.id == id1)
			msg += " in " + tournament.name;
	gui->quickMsg(msg);
}