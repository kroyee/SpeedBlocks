#ifndef GUI_H
#define GUI_H

#include <TGUI/TGUI.hpp>
#include "packetcompress.h"
#include "ScrollList.h"

class optionSet;
class soundBank;
class gamePlay;
class network;
class PacketCompress;
class textures;
class Resources;

class obsField;

class privChat {
public:
	tgui::ChatBox::Ptr chatBox;
	sf::String name;
};

class clientInfo {
public:
	sf::Uint16 id;
	sf::String name;
};

enum GameStates { MainMenu, CountDown, Game, GameOver, Replay, Practice };

class UI {
public:
	UI(sf::RenderWindow& window_, gamePlay& game_);

	tgui::Font typewriter;
	tgui::Font printFont2;

	tgui::Gui gui;
	tgui::Theme::Ptr themeTG;
	tgui::Theme::Ptr themeBB;

	std::vector<privChat> privChats;
	ScrollList roomList;
	ScrollList tournamentList;

	std::list<clientInfo> clientList;

	Resources& resources;
	optionSet& options;
	gamePlay& game;
	network& net;

	sf::RenderWindow* window;

	bool training;
	bool playonline;
	bool setkey;
	bool adjPieces;
	bool updPieces;
	bool chatFocused;
	bool inroom;
	bool away;

	sf::Keyboard::Key* key;
	tgui::Button::Ptr button;

	sf::Texture texture[7];
	sf::Sprite piece[7];

	sf::Uint16 linesSent, garbageCleared, linesBlocked;

	sf::Uint16 clientVersion;

	short scoreRows;

	sf::Time quickMsgTime;
	sf::Time udpPortTime;
	sf::Time pingTime;
	sf::Time spamTime;
	sf::Clock delayClock;

	bool udpConfirmed, pingReturned;
	sf::Uint8 pingIdCount;

	sf::Text ping, frameRate, inputRate, longestFrame, performanceLabel;
	sf::Uint8 pingColor, frameRateColor, inputRateColor, longestFrameColor;

	GameStates gamestate;

	short spamCount;

	void createAllWidgets();

	void setKey(tgui::Button::Ptr butt, sf::Keyboard::Key& skey);
	void putKey(sf::Event& event);
	void changeName(const sf::String& name);
	void initSprites();
	void Options();
	void mainMenu();
	void quitGame();
	void playOnline();
	void rotPiece(short i);
	void colPiece(short i);
	void otabSelect(std::string tab);
	void vidSlide(short i);
	void fsChecked(bool i);
	void applyVideo();
	void sndChecked(bool i);
	void volSlide(short i, short vol);
	void setBool(bool& var);
	void igtabSelect(const std::string& tab);
	void chattabSelect(const std::string& tab);
	void opTabSelect(const std::string& tab);
	void Training();
	void setGameState(GameStates state);

	void makeRoomList();
	void addRoom();
	void joinRoom(sf::Uint16);

	void makeClientList();
	void makeLobbyList();
	void addClient();
	void removeClient();

	void Chat();
	void Score();

	void privMsg(const sf::String& name, const sf::String& msg);
	void roomMsg(const sf::String& name, const sf::String& msg);
	void lobbyMsg(const sf::String& name, const sf::String& msg);
	void sendMsg(const sf::String& to, const sf::String& msg);
	void chatFocus(bool i);

	void scoreRow();
	void clearScore();

	void ausY();
	void ausN();

	void quickMsg(const sf::String& msg);

	void changeServerAdd(sf::String);

	void login(const sf::String&, const sf::String&, sf::Uint8);
	void closeLogin();
	void disconnect();

	void bugReport();
	void minimize(tgui::ChildWindow::Ptr);
	void close(tgui::ChildWindow::Ptr);
	void sendReport(tgui::ChildWindow::Ptr);

	void createRoom(const sf::String&, const sf::String&);

	void setFrameRate(int fr);
	void setInputRate(int fc);
	void setLongestFrame(sf::Time& lf);
	void setPing(sf::Time& pingResult);
	void drawPerformanceOutput();

	//GameFIeldDrawer stuff

	std::list<obsField> fields;

	sf::Clock sclock;
	obsField* scaleup;

	float currentR;

	PacketCompress compressor;
	sf::Time gamedata;
	sf::Uint8 gamedatacount;
	sf::Uint16 myId;

	sf::String unknown;

	void addField(obsField& field);
	void removeField(sf::Uint16 id);
	void updateField(obsField& field);
	void calFieldPos();

	void removeAllFields();
	void leaveRoom();

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

	void sendGameData();
	void sendGameOver();
	void sendGameWinner();

	void goAway();
	void unAway();

	const sf::String& getName(sf::Uint16);

	void delayCheck();

	//Send packet functions
	void sendPacket0(sf::Uint16 id);
	void sendPacket1();
	void sendPacket2(const sf::String& name, const sf::String& pass, sf::Uint8 guest);
	void sendPacket3();
	void sendPacket4();
	void sendPacket5(sf::Uint8 amount);
	void sendPacket6(sf::Uint8 amount);
	void sendPacket7(sf::Uint8 amount);
	void sendPacket8();
	void sendPacket9();
	void sendPacket10(const sf::String& to, const sf::String& msg);
	void sendPacket11(const sf::String& name, sf::Uint8 maxPlayers);
	void sendPacket99();
	void sendPacket100();
	void sendPacket102(sf::Uint8 pingId);
};

sf::String SFKeyToString(unsigned int keycode);
sf::Color pColor(short i);

#endif