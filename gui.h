#ifndef GUI_H
#define GUI_H

#include <TGUI/TGUI.hpp>
#include "packetcompress.h"

class optionSet;
class soundBank;
class gamePlay;
class network;
class PacketCompress;
class textures;

class obsField;

class privChat {
public:
	tgui::ChatBox::Ptr chatBox;
	sf::String name;
};

class playRoom {
public:
	sf::String name;
	sf::Uint8 currentPlayers;
	sf::Uint8 maxPlayers;
	sf::Uint16 id;
	tgui::Button::Ptr button;
	tgui::Label::Ptr label;
};

enum GameStates { MainMenu, CountDown, Game, GameOver };

class UI {
public:
	UI(sf::RenderWindow&, sf::Font&, sf::Font&, optionSet&, soundBank&, gamePlay&, network&, textures&);

	tgui::Font typewriter;
	tgui::Font printFont2;
	sf::Font* printFont;

	tgui::Gui gui;
	tgui::Theme::Ptr themeTG;
	tgui::Theme::Ptr themeBB;

	std::vector<privChat> privChats;
	std::list<playRoom> playRooms;

	optionSet* options;
	soundBank* sounds;
	gamePlay* game;
	network* net;
	textures* textureBase;

	sf::RenderWindow* window;

	bool training;
	bool playonline;
	bool quit;
	bool setkey;
	bool adjPieces;
	bool updPieces;
	bool chatFocused;
	bool inroom;
	bool startgame;
	bool startcount;
	bool disconnect;
	bool away;

	sf::Keyboard::Key* key;
	tgui::Button::Ptr button;

	sf::Texture texture[7];
	sf::Sprite piece[7];

	sf::Uint16 linesSent, garbageCleared, linesBlocked;

	sf::Uint16 clientVersion;

	short scoreRows;

	sf::Clock quickMsgClock;

	GameStates gamestate;

	void setKey(tgui::Button::Ptr butt, sf::Keyboard::Key& skey);
	void putKey(sf::Event& event);
	void changeName(const sf::String& name);
	void initSprites();
	void Options();
	void mainMenu();
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

	void addRoom(const sf::String& name, sf::Uint8 curr, sf::Uint8 max, sf::Uint16 id);
	void removeRoom(sf::Uint16 id);
	void removeAllRooms();
	void setRoomPos();
	void roomScrolled(int i);
	void joinRoom(sf::Uint16);

	void Chat();
	void Score();

	void privMsg(const sf::String& name, const sf::String& msg);
	void roomMsg(const sf::String& name, const sf::String& msg);
	void lobbyMsg(const sf::String& name, const sf::String& msg);
	void sendMsg(const sf::String& to, const sf::String& msg);
	void chatFocus(bool i);

	void scoreRow(sf::String&&, short, short, short, short, short, float, short, short, short);
	void clearScore();

	void ausY();
	void ausN();

	void quickMsg(const sf::String& msg);

	void changeServerAdd(sf::String);

	void login(const sf::String&, const sf::String&, sf::Uint8);
	void closeLogin();

	void bugReport();
	void minimize(tgui::ChildWindow::Ptr);
	void close(tgui::ChildWindow::Ptr);
	void sendReport(sf::String, sf::String, sf::String, sf::String, tgui::ChildWindow::Ptr);

	void createRoom(const sf::String&, const sf::String&);

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
	void leaveRoom();

	void resetOppFields();

	void drawOppField(obsField& field);
	void drawFields();

	void handleEvent(sf::Event event);
	void handlePacket();

	void sendGameData();
	void sendGameOver();
	void sendGameWinner();
	void sendGameState();

	void goAway();
	void unAway();

	sf::String getName(sf::Uint16);
};

sf::String SFKeyToString(unsigned int keycode);
sf::Color pColor(short i);

#endif