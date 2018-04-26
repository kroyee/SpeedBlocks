#include "OnlineplayUI.h"
#include "GameSignals.h"
#include "Resources.h"
#include <SFML/Network.hpp>
using std::to_string;
using std::cout;
using std::endl;

static auto& SendPacket = Signal<void, sf::Packet&>::get("SendPacket");
static auto& SetAreYouSure = Signal<void, const std::string&>::get("SetAreYouSure");
static auto& AddAlert = Signal<void, const std::string&>::get("AddAlert");

OnlineplayUI::OnlineplayUI(sf::Rect<int> _pos, Resources& _res) :
GuiBase(_pos, _res),
roomList(sf::Rect<int>(0,100,450,500), _res, panel),
tournamentList(sf::Rect<int>(0,100,450,500), _res, panel),
tournamentPanel(sf::Rect<int>(0,100,960,500), _res, panel, *this),
challengesUI(sf::Rect<int>(0,100,960,500), _res, panel)
{

	opTab = resources.gfx->load("Tab");
	opTab->add("Rooms");
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

	roomSidePanel = tgui::Panel::create();
	roomSidePanel->setPosition(460,100);
	roomSidePanel->setSize(400,500);
	roomSidePanel->setBackgroundColor(sf::Color(255,255,255,0));
	roomSidePanel->hide();
	panel->add(roomSidePanel);

	tgui::Button::Ptr widget8 = resources.gfx->load("Button");
	widget8->setPosition(20,20);
	widget8->setSize(320,75);
	widget8->setText("Create Room");
	widget8->connect("pressed", &OnlineplayUI::createRoomPressed, this);
	roomSidePanel->add(widget8);

	matchButton = resources.gfx->load("Button");
	matchButton->setPosition(20, 200);
	matchButton->setSize(320, 75);
	matchButton->setText("Join 1vs1 matchmaking");
	matchButton->connect("pressed", &OnlineplayUI::matchmakingPressed, this);
	roomSidePanel->add(matchButton);

	matchQueueing = resources.gfx->load("Label");
	matchQueueing->setPosition(50,290);
	matchQueueing->setText("In queue: 0");
	matchQueueing->setTextSize(18);
	roomSidePanel->add(matchQueueing);

	matchPlaying = resources.gfx->load("Label");
	matchPlaying->setPosition(220,290);
	matchPlaying->setText("Playing: 0");
	matchPlaying->setTextSize(18);
	roomSidePanel->add(matchPlaying);

	tournamentSidePanel = tgui::Panel::create();
	tournamentSidePanel->setPosition(460,100);
	tournamentSidePanel->setSize(400,500);
	tournamentSidePanel->setBackgroundColor(sf::Color(255,255,255,0));
	tournamentSidePanel->hide();
	panel->add(tournamentSidePanel);

	tgui::Button::Ptr widget0 = resources.gfx->load("Button");
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

	tgui::Label::Ptr widget2 = resources.gfx->load("Label");
	widget2->setPosition(70,40);
	widget2->setText("Tournament name");
	widget2->setTextSize(18);
	createTournamentPanel->add(widget2);

	tgui::Label::Ptr widget3 = resources.gfx->load("Label");
	widget3->setPosition(87,100);
	widget3->setText("Sets");
	widget3->setTextSize(18);
	createTournamentPanel->add(widget3);

	tgui::Label::Ptr widget4 = resources.gfx->load("Label");
	widget4->setPosition(340,100);
	widget4->setText("Rounds");
	widget4->setTextSize(18);
	createTournamentPanel->add(widget4);

	tournamentName = resources.gfx->load("EditBox");
	tournamentName->setPosition(255,37);
	tournamentName->setSize(260,30);
	createTournamentPanel->add(tournamentName);

	sets = resources.gfx->load("EditBox");
	sets->setPosition(140,97);
	sets->setSize(90,30);
	sets->setInputValidator(tgui::EditBox::Validator::UInt);
	createTournamentPanel->add(sets);

	rounds = resources.gfx->load("EditBox");
	rounds->setPosition(420,97);
	rounds->setSize(90,30);
	rounds->setInputValidator(tgui::EditBox::Validator::UInt);
	createTournamentPanel->add(rounds);

	tgui::Button::Ptr widget6 = resources.gfx->load("Button");
	widget6->setPosition(229,173);
	widget6->setSize(140,40);
	widget6->setText("Create");
	widget6->connect("pressed", &OnlineplayUI::createTournament, this);
	createTournamentPanel->add(widget6);

	tgui::Button::Ptr widget7 = resources.gfx->load("Button");
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

	tgui::Label::Ptr NorL = resources.gfx->load("Label");
	NorL->setPosition(30, 30);
	NorL->setText("Room name");
	CreateRoom->add(NorL);

	tgui::EditBox::Ptr NorE = resources.gfx->load("EditBox");
	NorE->setPosition(25, 60);
	NorE->setSize(250, 40);
	CreateRoom->add(NorE);

	tgui::Label::Ptr NopL = resources.gfx->load("Label");
	NopL->setPosition(430, 30);
	NopL->setText("Max players (0=unlimited)");
	CreateRoom->add(NopL);

	tgui::EditBox::Ptr NopE = resources.gfx->load("EditBox");
	NopE->setPosition(425, 60);
	NopE->setSize(250, 40);
	NopE->setInputValidator(tgui::EditBox::Validator::UInt);
	CreateRoom->add(NopE);

	tgui::Button::Ptr CrB = resources.gfx->load("Button");
	CrB->setPosition(300, 150);
	CrB->setText("Create");
	CrB->setSize(100, 40);
	CrB->connect("Pressed", &OnlineplayUI::createRoom, this, std::bind(&tgui::EditBox::getText, NorE), std::bind(&tgui::EditBox::getText, NopE));
	CreateRoom->add(CrB);

	connectSignal("SetRoomListTime", &OnlineplayUI::setRoomListTime, this);

	Net::takePacket(16, &OnlineplayUI::makeRoomList, this);
	Net::takePacket(17, &OnlineplayUI::addRoom, this);
	Net::takePacket(18, [&](sf::Packet &packet){
		uint16_t id;
		packet >> id;
		roomList.removeItem(id);
	});
	Net::takePacket(22, &OnlineplayUI::makeTournamentList, this);

	Net::takeSignal(1, &OnlineplayUI::alertMsg, this);
}

void OnlineplayUI::opTabSelect(const std::string& tab) {
	if (tab == "Rooms") {
		hideAllPanels();
		roomList.show();
		roomSidePanel->show();
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
		SetAreYouSure("Leave the server?");
	}
}

void OnlineplayUI::hideAllPanels(bool keepTournamentOpen) {
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

void OnlineplayUI::createRoom(const std::string& name, const std::string& maxplayers) {
	if (!name.size())
		return;
	if (!maxplayers.size())
		return;
	sf::Packet packet;
	packet << (uint8_t)11 << name << (uint8_t)std::stoi(maxplayers);
	SendPacket(packet);
	hideAllPanels();
	roomList.show();
	roomSidePanel->show();
}

void OnlineplayUI::makeRoomList(sf::Packet &packet) {
	uint8_t roomCount;

	packet >> roomCount;
	auto scrollpos = roomList.scroll->getValue();
	roomList.removeAllItems();

	for (int i=0; i<roomCount; i++)
		addRoom(packet);

	roomList.scroll->setValue(scrollpos);

	uint16_t inqueue, inplay;
	packet >> inqueue >> inplay;
	matchQueueing->setText("In queue: " + to_string(inqueue));
	matchPlaying->setText("Playing: " + to_string(inplay));
}

void OnlineplayUI::addRoom(sf::Packet &packet) {
	std::string name;
	uint8_t maxPlayers, currentPlayers;
	uint16_t id;
	packet >> id >> name >> currentPlayers >> maxPlayers;
	std::string roomlabel = to_string(currentPlayers);
	if (maxPlayers)
		roomlabel += "/" + to_string(maxPlayers);
	roomlabel+= " players";
	roomList.addItem(name, roomlabel, id);
}

void OnlineplayUI::makeTournamentList(sf::Packet &packet) {
	uint8_t tournamentCount;

	packet >> tournamentCount;
	auto scrollpos = tournamentList.scroll->getValue();
	tournamentList.removeAllItems();

	for (int i=0; i<tournamentCount; i++)
		addTournament(packet);

	tournamentList.scroll->setValue(scrollpos);
}

void OnlineplayUI::addTournament(sf::Packet &packet) {
	std::string name;
	uint8_t status;
	uint16_t id, players;
	packet >> id >> name >> status >> players;
	std::string label;
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
		SendSignal(21);
	else
		SendSignal(22);
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
	uint8_t setcount = stoi(sets->getText().toAnsiString());
	uint8_t roundcount = stoi(rounds->getText().toAnsiString());
	sf::Packet packet;
	packet << (uint8_t)21 << tournamentName->getText() << setcount << roundcount;
	SendPacket(packet);
	back();
	updateTournamentListTime -= sf::seconds(5);
}

void OnlineplayUI::alertMsg(const uint16_t id1) {
	std::string msg = "Tournament game ready";
	for (auto&& tournament : tournamentList.items)
		if (tournament.id == id1)
			msg += " in " + tournament.name;
	AddAlert(msg);
}

void OnlineplayUI::setRoomListTime() {
	updateTournamentListTime = resources.delayClock.getElapsedTime() + sf::seconds(5);
}
