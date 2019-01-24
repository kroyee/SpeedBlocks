#ifndef SERVERUI_H
#define SERVERUI_H

#include "GuiBase.h"

namespace sf {
class Packet;
}

class ServerUI : public GuiBase {
   public:
    os::ListBox LobbyList;
    os::Label motd;

    ServerUI(sf::Rect<int> _pos, Resources &_res, os::Panel &parent);

    void makeClientList(sf::Packet &packet);
    void makeLobbyList();
    void addClient(sf::Packet &packet);
    void putClient(uint16_t id, const std::string &name);
    void removeClient(sf::Packet &packet);
    void linkPressed(uint8_t type);
    void clear();
};

#endif