#include "OnlineplayUI.h"
#include <SFML/Network.hpp>
#include "GameSignals.h"
#include "Packets.hpp"
#include "Resources.h"

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

    auto add_room = [&](const RoomInfo& room) {
        std::string roomlabel = std::to_string(room.currentPlayers);
        if (room.maxPlayers) roomlabel += "/" + std::to_string(room.maxPlayers);
        roomlabel += " players";
        roomList.addItem(room.name, roomlabel, room.id);
    };

    PM::handle_packet([&](const NP_RoomList& p) {
        auto scrollpos = roomList.scroll->getValue();
        roomList.removeAllItems();

        for (auto& room : p.rooms) add_room(room);

        roomList.scroll->setValue(scrollpos);
    });
    PM::handle_packet([&](const NP_RoomAdd& p) { add_room(p.room); });
    PM::handle_packet([&](const NP_RoomRemove& p) { roomList.removeItem(p.id); });
    PM::handle_packet([&](const NP_TournamentList& p) {
        auto scrollpos = tournamentList.scroll->getValue();
        tournamentList.removeAllItems();

        for (auto& tournament : p.tournaments) {
            const std::array<std::string, 5> labels = {"Sign Up - ", "Pending - ", "Started - ", "Finished - ", "Aborted - "};
            auto label = labels[tournament.status] + std::to_string(tournament.players) + " players";
            tournamentList.addItem(tournament.name, label, tournament.id);
        }

        tournamentList.scroll->setValue(scrollpos);
    });
    PM::handle_packet([&](const NP_Alert& p) { AddAlert(p.text); });
    PM::handle_packet([&](const NP_TournamentGameReady& p) {
        std::string msg = "Tournament game ready";
        for (auto&& tournament : tournamentList.items)
            if (tournament.id == p.tournament_id) msg += " in " + tournament.name;
        AddAlert(msg);
    });
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
    NP_CreateRoom packet{name, static_cast<uint8_t>(std::stoi(maxplayers))};
    TCP.write(packet);
    hideAllPanels();
    roomList.show();
    roomSidePanel.show();
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
    NP_CreateTournament packet;
    packet.sets = stoi(sets->getText().toAnsiString());
    packet.rounds = stoi(rounds->getText().toAnsiString());
    packet.name = tournamentName->getText();
    TCP.write(packet);
    back();
    updateTournamentListTime -= sf::seconds(5);
}

void OnlineplayUI::setRoomListTime() { updateTournamentListTime = resources.delayClock.getElapsedTime() + sf::seconds(5); }
