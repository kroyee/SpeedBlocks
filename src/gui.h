#ifndef GUI_H
#define GUI_H

#include <TGUI/TGUI.hpp>
#include "packetcompress.h"
#include "Resources.h"

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
class GameplayUI;
class OnlineplayUI;
class AreYouSure;
class PerformanceOutput;
class BugReport;
class ChallengesGameUI;
class ReplayUI;

class obsField;

class UI {
public:
	UI(sf::RenderWindow& window_, gamePlay& game_);
	~UI();

	tgui::Gui tGui;
	tgui::Theme::Ptr themeTG;
	tgui::Theme::Ptr themeBB;

	Menu* mainMenu;
	LoginBox* loginBox;
	GameOptions* gameOptions;
	Connecting* connectingScreen;
	GameplayUI* gameplayUI;
	OnlineplayUI* onlineplayUI;
	AreYouSure* areYouSure;
	PerformanceOutput* performanceOutput;
	BugReport* bugReport;
	tgui::Label::Ptr QuickMsg;
	ChallengesGameUI* challengesGameUI;
	ReplayUI* replayUI;

	Resources& resources;
	optionSet& options;
	gamePlay& game;
	network& net;

	sf::RenderWindow* window;

	bool playonline;
	bool chatFocused;
	bool inroom;
	bool away;

	sf::Uint16 linesSent, garbageCleared, linesBlocked;

	sf::Uint16 clientVersion;

	sf::Time quickMsgTime;
	sf::Time udpPortTime;
	sf::Time spamTime;
	sf::Clock delayClock;

	bool udpConfirmed;

	GameStates& gamestate;

	short spamCount;

	void setGameState(GameStates state);

	void Chat();
	void Score();

	void joinRoom(sf::Uint16);
	void leaveRoom();

	void getMsg();
	void chatMsg(const sf::String& to, const sf::String& msg);
	void sendMsg(const sf::String& to, const sf::String& msg);
	void chatFocus(bool i);

	void quickMsg(const sf::String& msg);
	void receiveRecording();

	void disconnect();

	//GameFIeldDrawer stuff

	std::list<obsField> fields;

	sf::Clock sclock;
	obsField* scaleup;

	float currentR;

	PacketCompress compressor;
	sf::Time gamedata;
	sf::Uint8 gamedatacount;
	sf::Uint16 myId;

	void addField(obsField& field);
	void removeField(sf::Uint16 id);
	void updateField(obsField& field);
	void calFieldPos();

	void removeAllFields();

	void resetOppFields();

	void drawOppField(obsField& field);
	void drawFields();

	void handleEvent(sf::Event& event);

	void gameInput(sf::Event& event);
	void windowEvents(sf::Event& event);
	void resizeWindow(sf::Event& event);
	
	void enlargePlayfield(sf::Event& event);
	void keyEvents(sf::Event& event);
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
};

#endif