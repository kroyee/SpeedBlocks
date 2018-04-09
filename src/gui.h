#ifndef GUI_H
#define GUI_H

#include <TGUI/TGUI.hpp>
#include "packetcompress.h"
#include "Resources.h"
#include "GameFieldDrawer.h"
#include "PingHandle.h"
#include "CountdownHandle.h"
#include <functional>
#include <memory>

class optionSet;
class soundBank;
class gamePlay;
class network;
class PacketCompress;
class textures;
class Resources;
struct GuiElements;
class obsField;
class UIBaseState;

class UI {
public:
	UI(sf::RenderWindow& window_, gamePlay& game_);
	~UI();

	Resources& resources;
	optionSet& options;
	gamePlay& game;
	PingHandle ping;
	CountdownHandle countdown;

	GuiElements *guiElements;

	sf::RenderWindow* window;

	bool away;

	sf::Time quickMsgTime;
	sf::Time udpPortTime;
	sf::Clock& delayClock;

	GameStates& gamestate;
	std::unique_ptr<UIBaseState> state;

	uint16_t& myId;

	void setCountdown(sf::Packet &packet);

	void joinRoom(int);
	void leaveRoom();

	void chatFocus(bool i);

	void receiveRecording(sf::Packet &packet);

	void getAlert();

	bool handleEvent(sf::Event& event);

	void getGameState(sf::Packet&);

	void delayCheck();

	void darkTheme();
	void lightTheme();
	void setWidgetTextColor(sf::Color color);

	void setOnChatFocus(const std::vector<tgui::Widget::Ptr> widgets);

	void joinRoomResponse(sf::Packet &packet);
};

#endif