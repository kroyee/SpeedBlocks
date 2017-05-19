#ifndef ONLINEPLAYUI_H
#define ONLINEPLAYUI_H

#include "guiBase.h"
#include "ScrollList.h"

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

	std::list<clientInfo> clientList;

	void create(sf::Rect<int> _pos, UI* _ui);

	void opTabSelect(const std::string& tab);

	void createRoom(const sf::String& name, const sf::String& maxplayers);
	void makeRoomList();
	void addRoom();

	void makeClientList();
	void makeLobbyList();
	void addClient();
	void removeClient();
};

#endif