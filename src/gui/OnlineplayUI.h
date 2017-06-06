#ifndef ONLINEPLAYUI_H
#define ONLINEPLAYUI_H

#include "guiBase.h"
#include "ScrollList.h"
#include "TournamentUI.h"

class UI;

class clientInfo {
public:
	sf::Uint16 id;
	sf::String name;
};

class OnlineplayUI : public guiBase {
public:
	tgui::Tab::Ptr opTab;
	tgui::Panel::Ptr ServerLobby;
	tgui::ChatBox::Ptr Lobby;
	tgui::EditBox::Ptr ChatBox;
	tgui::ListBox::Ptr LobbyList;
	tgui::Panel::Ptr CreateRoom;

	ScrollList roomList;
	ScrollList tournamentList;

	TournamentUI tournamentPanel;

	tgui::Panel::Ptr tournamentSidePanel;

	tgui::Panel::Ptr createTournamentPanel;
	tgui::EditBox::Ptr tournamentName;
	tgui::EditBox::Ptr sets;
	tgui::EditBox::Ptr rounds;

	std::list<clientInfo> clientList;

	sf::Uint8 backTo;

	void create(sf::Rect<int> _pos, UI* _gui);

	void opTabSelect(const std::string& tab);
	void hideAllPanels(bool keepTournamentOpen = false);

	void createRoom(const sf::String& name, const sf::String& maxplayers);
	void makeRoomList();
	void addRoom();

	void makeClientList();
	void makeLobbyList();
	void addClient();
	void removeClient();

	void makeTournamentList();
	void addTournament();

	void createTournamentPressed();
	void refreshTournamentPressed();

	void createTournament();

	void alertMsg(const sf::Uint16 id1);

	void back();
};

#endif