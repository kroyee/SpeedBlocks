#ifndef CHATSCREEN_H
#define CHATSCREEN_H

#include <TGUI/TGUI.hpp>
#include <deque>
#include "guiBase.h"

class UI;

class ChatScreen : public guiBase {
private:
	tgui::EditBox::Ptr input;
	tgui::ChatBox::Ptr chatBox;
	tgui::ChatBox::Ptr chatBoxNoLobby;
	tgui::ChatBox::Ptr fadingChatBox;
	tgui::CheckBox::Ptr hideLobby;
	tgui::Label::Ptr sendToLabel;
	std::deque<sf::Time> fadeTime;
	sf::Time spamTime;
	short spamCount;
	sf::String to;
	sf::String privto;
	bool hideLobbyChat;
	sf::Color t1, t2, t3, t4, t5;
public:
	ChatScreen(sf::Rect<int> _pos, UI* _gui);
	void activate();
	void deactivate();
	void send();
	void sendMsg(const sf::String& to, const sf::String& msg);
	void addLine(const sf::String& msg, sf::Uint8 type);
	void getMsg();
	void fade(const sf::Time& t);
	bool handleEvent(sf::Event& event);
	bool isActive();
	void focus();
	void sendTo(const sf::String& _to, bool force=false);
	void toggleSendTo();
	void dark();
	void light();
};

#endif