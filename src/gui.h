#ifndef GUI_H
#define GUI_H

#include <TGUI/TGUI.hpp>
#include "packetcompress.h"
#include "Resources.h"
#include "GameFieldDrawer.h"
#include "PingHandle.h"
#include "CountdownHandle.h"
#include <functional>

class optionSet;
class soundBank;
class gamePlay;
class network;
class PacketCompress;
class textures;
class Resources;
class GuiElements;
class obsField;

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

	sf::Uint16 linesSent, garbageCleared, linesBlocked;

	sf::Time quickMsgTime;
	sf::Time udpPortTime;
	sf::Clock& delayClock;

	GameStates& gamestate;

	void setGameState(GameStates state);

	void setCountdown(sf::Packet &packet);

	void joinRoom(int);
	void leaveRoom();

	void chatFocus(bool i);

	void receiveRecording(sf::Packet &packet);

	void getAlert();

	sf::Uint16& myId;

	bool handleEvent(sf::Event& event);

	void getGameState(sf::Packet&);

	void delayCheck();

	void darkTheme();
	void lightTheme();
	void setWidgetTextColor(sf::Color color);

	void setOnChatFocus(const std::vector<tgui::Widget::Ptr> widgets);

	void joinRoomResponse(sf::Packet &packet);
	void getAuthResult(sf::Packet &packet);
};

#endif