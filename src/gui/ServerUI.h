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

    void makeLobbyList();
    void putClient(uint16_t id, const std::string &name);
    void linkPressed(uint8_t type);
    void clear();
};

#endif