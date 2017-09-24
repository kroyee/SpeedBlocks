#ifndef SERVERUI_H
#define SERVERUI_H

#include "guiBase.h"

namespace sf { class Packet; }

class ServerUI : public guiBase {
public:
	tgui::ListBox::Ptr LobbyList;
	tgui::Label::Ptr motd;

	ServerUI(sf::Rect<int> _pos, Resources& _res, tgui::Panel::Ptr parent);

	void makeClientList(sf::Packet &packet);
	void makeLobbyList();
	void addClient(sf::Packet &packet);
	void putClient(sf::Uint16 id, const sf::String& name);
	void removeClient(sf::Packet &packet);
	void linkPressed(sf::Uint8 type);
	void clear();
};

#endif