#ifndef GUI_H
#define GUI_H

#include <TGUI/TGUI.hpp>
#include "packetcompress.h"
#include "Resources.h"
#include "GameFieldDrawer.h"
#include "PingHandle.h"
#include "CountdownHandle.h"

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
class Connecting;
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

class obsField;

class UI {
public:
	UI(sf::RenderWindow& window_, gamePlay& game_);
	~UI();

	tgui::Gui tGui;
	tgui::Theme::Ptr themeTG;

	Menu* mainMenu;
	LoginBox* loginBox;
	GameOptions* gameOptions;
	Connecting* connectingScreen;
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

	Resources& resources;
	optionSet& options;
	gamePlay& game;
	network& net;
	PingHandle ping;
	CountdownHandle countdown;

	sf::RenderWindow* window;

	bool playonline;
	bool chatFocused;
	bool away;

	sf::Uint16 linesSent, garbageCleared, linesBlocked;

	sf::Uint16 clientVersion;

	sf::Time quickMsgTime;
	sf::Time udpPortTime;
	sf::Clock delayClock;

	bool udpConfirmed;

	GameStates& gamestate;

	void setGameState(GameStates state);

	void setCountdown();

	void joinRoom(sf::Uint16);
	void leaveRoom();

	void chatFocus(bool i);

	void quickMsg(const sf::String& msg);
	void receiveRecording();

	void iGotKicked(sf::Uint16 reason);

	void disconnect();

	GameFieldDrawer gameFieldDrawer;

	PacketCompress compressor;
	sf::Time gamedata;
	sf::Uint8 gamedatacount;
	sf::Uint16 myId;

	void handleEvent(sf::Event& event);

	void gameInput(sf::Event& event);
	void windowEvents(sf::Event& event);
	void resizeWindow(sf::Event& event);
	void toggleFullscreen();
	
	bool keyEvents(sf::Event& event);
	void scrollBar(sf::Event& event);

	void handlePacket();
	void handleSignal();

	void sendGameData();
	void sendGameState();
	void sendGameOver();
	void sendGameWinner();

	void goAway();
	void unAway();

	void ready();

	void delayCheck();

	void darkTheme();
	void lightTheme();
	void setWidgetTextColor(sf::Color color);

	void setOnChatFocus(const std::vector<tgui::Widget::Ptr> widgets);
};

#endif