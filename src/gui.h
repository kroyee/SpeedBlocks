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

class Menu;
class LoginBox;
class GameOptions;
class OnlineplayUI;
class AreYouSure;
class PerformanceOutput;
class BugReport;
class ChallengesGameUI;
class ReplayUI;
class GameStandings;
class ChatScreen;
class SlideMenu;
class ScoreScreen;
class AnimatedBackground;
class ServerUI;
class AlertsUI;
class guiBase;

class obsField;

class UI {
public:
	UI(sf::RenderWindow& window_, gamePlay& game_);
	~UI();

	Menu* mainMenu;
	LoginBox* loginBox;
	GameOptions* gameOptions;
	OnlineplayUI* onlineplayUI;
	AreYouSure* areYouSure;
	PerformanceOutput* performanceOutput;
	BugReport* bugReport;
	tgui::Label::Ptr QuickMsg;
	ChallengesGameUI* challengesGameUI;
	ReplayUI* replayUI;
	GameStandings* gameStandings;
	ChatScreen* chatScreen;
	SlideMenu* slideMenu;
	ScoreScreen* scoreScreen;
	AnimatedBackground* animatedBackground;
	ServerUI* serverUI;
	AlertsUI* alertsUI;

	std::vector<guiBase*> guiElements;

	Resources& resources;
	optionSet& options;
	gamePlay& game;
	network& net;
	PingHandle ping;
	CountdownHandle countdown;

	sf::RenderWindow* window;

	bool chatFocused;
	bool away;

	sf::Uint16 linesSent, garbageCleared, linesBlocked;

	sf::Time quickMsgTime;
	sf::Time udpPortTime;
	sf::Clock& delayClock;

	bool udpConfirmed;

	GameStates& gamestate;

	void setGameState(GameStates state);

	void setCountdown(sf::Packet &packet);

	void joinRoom(int);
	void leaveRoom();

	void chatFocus(bool i);

	void quickMsg(const sf::String& msg);
	void receiveRecording(sf::Packet &packet);

	void iGotKicked(sf::Uint16 reason);
	void getAlert();

	void disconnect();

	GameFieldDrawer gameFieldDrawer;

	PacketCompress compressor;
	sf::Time gamedata;
	sf::Uint8 gamedatacount;
	sf::Uint16& myId;

	void handleEvent(sf::Event& event);

	void gameInput(sf::Event& event);
	void windowEvents(sf::Event& event);
	void resizeWindow(sf::Event& event);
	void toggleFullscreen();
	
	bool keyEvents(sf::Event& event);
	void scrollBar(sf::Event& event);

	void handlePacket();
	void handleSignal();

	void getGameState(sf::Packet&);

	void sendGameData();
	void sendGameState();
	void sendGameOver();
	void sendGameWinner();

	void delayCheck();

	void darkTheme();
	void lightTheme();
	void setWidgetTextColor(sf::Color color);

	void setOnChatFocus(const std::vector<tgui::Widget::Ptr> widgets);

	void showElement(int elem);
	void hideElement(int elem);
	void enableElement(int elem);
	void disableElement(int elem);
	bool isVisible(int elem);

	void joinRoomResponse(sf::Packet &packet);
	void getAuthResult(sf::Packet &packet);
};

#endif