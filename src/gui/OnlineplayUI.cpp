#include "OnlineplayUI.h"
#include <SFML/Network.hpp>
#include "GameSignals.h"
#include "Resources.h"

static auto& SendPacket = Signal<void, sf::Packet&>::get("SendPacket");
static auto& SetAreYouSure = Signal<void, const std::string&>::get("SetAreYouSure");
static auto& AddAlert = Signal<void, const std::string&>::get("AddAlert");

OnlineplayUI::OnlineplayUI(sf::Rect<int> _pos, Resources& _res)
    : GuiBase(_pos, _res),
      roomList(sf::Rect<int>(0, 100, 450, 500), _res, panel),
      tournamentList(sf::Rect<int>(0, 100, 450, 500), _res, panel),
      tournamentPanel(sf::Rect<int>(0, 100, 960, 500), _res, panel, *this),
      challengesUI(sf::Rect<int>(0, 100, 960, 500), _res, panel) {
    opTab.add("Rooms")
        .add("Tournaments")
        .add("Challenges")
        .add("Back")
        .tab_height(50)
        .pos(50, 20)
        .select(1)
        .connect("TabSelected", &OnlineplayUI::opTabSelect, this)
        .opacity(0.7)
        .add_to(panel);

    sf::Rect<int> pos(0, 100, 450, 500);

    roomSidePanel.pos(460, 100).size(400, 500).hide().add_to(panel);

    os::Button().pos(20, 20).size(320, 75).text("Create Room").connect("pressed", &OnlineplayUI::createRoomPressed, this).add_to(roomSidePanel);

    matchButton.pos(20, 200).size(320, 75).text("Join 1vs1 matchmaking").connect("pressed", &OnlineplayUI::matchmakingPressed, this).add_to(roomSidePanel);

    matchQueueing.pos(50, 290).text("In queue: 0").text_size(18).add_to(roomSidePanel);

    matchPlaying.pos(220, 290).text("Playing: 0").text_size(18).add_to(roomSidePanel);

    tournamentSidePanel.pos(460, 100).size(400, 500).hide().add_to(panel);

    os::Button().pos(20, 20).size(320, 75).text("Create Tournament").connect("pressed", &OnlineplayUI::createTournamentPressed, this).add_to(tournamentSidePanel);

    createTournamentPanel.pos(0, 100).size(960, 500).hide().add_to(panel);

    os::Label().pos(70, 40).text("Tournament name").text_size(18).add_to(createTournamentPanel);

    os::Label().pos(87, 100).text("Sets").text_size(18).add_to(createTournamentPanel);

    os::Label().pos(340, 100).text("Rounds").text_size(18).add_to(createTournamentPanel);

    tournamentName.pos(255, 37).size(260, 30).add_to(createTournamentPanel);

    sets.pos(140, 97).size(90, 30).num().add_to(createTournamentPanel);

    rounds.pos(420, 97).size(90, 30).num().add_to(createTournamentPanel);

    os::Button().pos(229, 173).size(140, 40).text("Create").connect("pressed", &OnlineplayUI::createTournament, this).add_to(createTournamentPanel);

    os::Button().pos(400, 173).size(140, 40).text("Back").connect("pressed", &OnlineplayUI::back, this).add_to(createTournamentPanel);

    CreateRoom.pos(0, 100).size(960, 500).hide().add_to(panel);

    os::Label().pos(30, 30).text("Room name").add_to(CreateRoom);
    os::Edit NorE;
    NorE.pos(25, 60).size(250, 40).add_to(CreateRoom);
    os::Label().pos(430, 30).text("Max players (0=unlimited)").add_to(CreateRoom);
    os::Edit NopE;
    NopE.pos(425, 60).size(250, 40).num().add_to(CreateRoom);
    os::Button()
        .pos(300, 150)
        .text("Create")
        .size(100, 40)
        .connect("Pressed", [&, NorE, NopE]() mutable { createRoom(NorE->getText(), NopE->getText()); })
        .add_to(CreateRoom);

    connectSignal("SetRoomListTime", &OnlineplayUI::setRoomListTime, this);

    Net::takePacket(16, &OnlineplayUI::makeRoomList, this);
    Net::takePacket(17, &OnlineplayUI::addRoom, this);
    Net::takePacket(18, [&](sf::Packet& packet) {
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
        roomSidePanel.show();
    } else if (tab == "Tournaments") {
        hideAllPanels();
        tournamentList.show();
        tournamentSidePanel.show();
    } else if (tab == "Create room") {
        hideAllPanels();
        CreateRoom.show();
    } else if (tab == "Challenges") {
        hideAllPanels();
        challengesUI.show();
    } else if (tab == "Back") {
        SetAreYouSure("Leave the server?");
    }
}

void OnlineplayUI::hideAllPanels(bool keepTournamentOpen) {
    roomList.hide();
    tournamentList.hide();
    CreateRoom.hide();
    if (!keepTournamentOpen) tournamentPanel.hide();
    tournamentSidePanel.hide();
    createTournamentPanel.hide();
    challengesUI.hide();
    roomSidePanel.hide();
}

void OnlineplayUI::createRoom(const std::string& name, const std::string& maxplayers) {
    if (!name.size()) return;
    if (!maxplayers.size()) return;
    sf::Packet packet;
    packet << (uint8_t)11 << name << (uint8_t)std::stoi(maxplayers);
    SendPacket(packet);
    hideAllPanels();
    roomList.show();
    roomSidePanel.show();
}

void OnlineplayUI::makeRoomList(sf::Packet& packet) {
    uint8_t roomCount;

    packet >> roomCount;
    auto scrollpos = roomList.scroll->getValue();
    roomList.removeAllItems();

    for (int i = 0; i < roomCount; i++) addRoom(packet);

    roomList.scroll->setValue(scrollpos);

    uint16_t inqueue, inplay;
    packet >> inqueue >> inplay;
    matchQueueing.text("In queue: " + std::to_string(inqueue));
    matchPlaying.text("Playing: " + std::to_string(inplay));
}

void OnlineplayUI::addRoom(sf::Packet& packet) {
    std::string name;
    uint8_t maxPlayers, currentPlayers;
    uint16_t id;
    packet >> id >> name >> currentPlayers >> maxPlayers;
    std::string roomlabel = std::to_string(currentPlayers);
    if (maxPlayers) roomlabel += "/" + std::to_string(maxPlayers);
    roomlabel += " players";
    roomList.addItem(name, roomlabel, id);
}

void OnlineplayUI::makeTournamentList(sf::Packet& packet) {
    uint8_t tournamentCount;

    packet >> tournamentCount;
    auto scrollpos = tournamentList.scroll->getValue();
    tournamentList.removeAllItems();

    for (int i = 0; i < tournamentCount; i++) addTournament(packet);

    tournamentList.scroll->setValue(scrollpos);
}

void OnlineplayUI::addTournament(sf::Packet& packet) {
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
    label += std::to_string(players) + " players";
    tournamentList.addItem(name, label, id);
}

void OnlineplayUI::createRoomPressed() {
    hideAllPanels();
    CreateRoom.show();
}

void OnlineplayUI::createTournamentPressed() {
    hideAllPanels();
    createTournamentPanel.show();
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
        tournamentSidePanel.show();
    }
}

void OnlineplayUI::createTournament() {
    if (!tournamentName->getText().getSize() || !sets->getText().getSize() || !rounds->getText().getSize()) return;
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
        if (tournament.id == id1) msg += " in " + tournament.name;
    AddAlert(msg);
}

void OnlineplayUI::setRoomListTime() { updateTournamentListTime = resources.delayClock.getElapsedTime() + sf::seconds(5); }
