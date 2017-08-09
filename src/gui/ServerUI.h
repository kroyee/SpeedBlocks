#ifndef SERVERUI_H
#define SERVERUI_H

#include "guiBase.h"

struct clientInfo {
public:
	sf::Uint16 id;
	sf::String name;
};

class ServerUI : public guiBase {
public:
	tgui::ListBox::Ptr LobbyList;
	tgui::Label::Ptr motd;

	std::list<clientInfo> clientList;

	void create(sf::Rect<int> _pos, UI* _gui, tgui::Panel::Ptr parent);

	void makeClientList();
	void makeLobbyList();
	void addClient();
	void putClient(sf::Uint16 id, const sf::String& name);
	void removeClient();
	void linkPressed(sf::Uint8 type);
	void clear();
};

#endif