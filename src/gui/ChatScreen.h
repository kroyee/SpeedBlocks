#ifndef CHATSCREEN_H
#define CHATSCREEN_H

#include <TGUI/TGUI.hpp>
#include <deque>
#include "GuiBase.h"

class Resources;

class ChatScreen : public GuiBase {
   private:
    os::Edit input;
    os::ChatBox chatBox;
    os::ChatBox chatBoxNoLobby;
    os::ChatBox fadingChatBox;
    os::CheckBox hideLobby;
    os::Label sendToLabel;
    std::deque<sf::Time> fadeTime;
    sf::Time spamTime;
    int spamCount;
    std::string to;
    std::string privto;
    bool hideLobbyChat;
    sf::Color t1, t2, t3, t4, t5;

   public:
    ChatScreen(sf::Rect<int> _pos, Resources& _res);
    void activate();
    void deactivate();
    void send();
    void sendMsg(const std::string& to, const std::string& msg);
    void addLine(const std::string& msg, uint8_t type);
    void fade(const sf::Time& t);
    bool handleEvent(sf::Event& event);
    bool isActive();
    void focus();
    void sendTo(const std::string& _to, bool force = false);
    void toggleSendTo();
    void dark();
    void light();
};

#endif