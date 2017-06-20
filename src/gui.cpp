#include "gui.h"
#include "sounds.h"
#include "optionSet.h"
#include "gamePlay.h"
#include "network.h"
#include "MainMenu.h"
#include "LoginBox.h"
#include "GameOptions.h"
#include "Connecting.h"
#include "GameplayUI.h"
#include "OnlineplayUI.h"
#include "AreYouSure.h"
#include "PerformanceOutput.h"
#include "BugReport.h"
#include "ChallengesGameUI.h"
#include "ReplayUI.h"
#include <iostream>
using std::cout;
using std::endl;
using std::move;

#ifdef __APPLE__
#include "ResourcePath.hpp"
#else
#include "EmptyResourcePath.h"
#endif

UI::UI(sf::RenderWindow& window_,
	gamePlay& game_)
    : tGui(window_),
      resources(game_.resources),
      options(game_.resources.options),
      game(game_),
      net(game_.resources.net),
      window(&window_),
      playonline(false),
      chatFocused(false),
      inroom(false),
      away(false),
      linesSent(0),
      garbageCleared(0),
      linesBlocked(0),
      clientVersion(0),
      udpConfirmed(false),
      gamestate(game_.resources.gamestate),
      //gamestate(MainMenu),
      spamCount(0),
      scaleup(0),
      gamedata(sf::seconds(0)),
      gamedatacount(0),
      myId(0) {

    compressor.game=&game;

    themeTG = tgui::Theme::create(resourcePath() + "media/TransparentGrey.txt");
	themeBB = tgui::Theme::create(resourcePath() + "media/BabyBlue.txt");

	sf::Rect<int> pos(0,0,960,600);
	mainMenu = new Menu;
	mainMenu->create(pos, this);
	mainMenu->show();

	gameOptions = new GameOptions;
	gameOptions->create(pos, this);
	gameplayUI = new GameplayUI;
	gameplayUI->create(pos, this);
	challengesGameUI = new ChallengesGameUI;
	challengesGameUI->create(pos, this);
	onlineplayUI = new OnlineplayUI;
	onlineplayUI->create(pos, this);

	pos.left=465; pos.top=530; pos.width=490; pos.height=70;
	replayUI = new ReplayUI;
	replayUI->create(pos, this);

	pos.left=280; pos.top=150; pos.width=400; pos.height=300;
	loginBox = new LoginBox;
	loginBox->create(pos, this);

	pos.left=330; pos.top=250; pos.width=300; pos.height=100;
	connectingScreen = new Connecting;
	connectingScreen->create(pos, this);

	areYouSure = new AreYouSure;
	areYouSure->create(pos, this);

	pos.left=847; pos.top=0; pos.width=113; pos.height=28;
	performanceOutput = new PerformanceOutput;
	performanceOutput->create(pos, this);
	if (options.performanceOutput)
		performanceOutput->show();

	QuickMsg = themeTG->load("Label");
	QuickMsg->setPosition(0,10);
	QuickMsg->setSize(960,90);
	QuickMsg->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Center);
	QuickMsg->setTextSize(32);
	QuickMsg->setTextColor(sf::Color::Red);
	QuickMsg->hide();
	QuickMsg->disable(false);
	tGui.add(QuickMsg);

	bugReport = new BugReport;
	bugReport->create(pos, this);
}

UI::~UI() {
	delete bugReport;
	delete areYouSure;
	delete performanceOutput;
	delete connectingScreen;
	delete loginBox;
	delete onlineplayUI;
	delete gameplayUI;
	delete gameOptions;
	delete mainMenu;
	delete challengesGameUI;
	delete replayUI;
}

void UI::joinRoom(sf::Uint16 id) {
	net.sendSignal(0, id);
	away=false;
	game.autoaway=false;
}

void UI::leaveRoom() {
	net.sendSignal(1);
	inroom=false;
	setGameState(MainMenu);
}

void UI::setGameState(GameStates state) {
	if (gamestate == MainMenu) { // Reset depending on what state we come from
		mainMenu->hide();
		onlineplayUI->hide();
		if (state != MainMenu) {
			gameplayUI->show();
			gameplayUI->InGameTab->select(0);
		}
	}
	else if (gamestate == Practice) {
		game.sendgameover = false;
		game.winner = false;
		game.autoaway = false;
		if (state == GameOver) {
			game.field.text.away=false;
			game.field.text.ready=true;
		}
	}

	if (state == MainMenu) { // Set depending on what state we are going into
		removeAllFields();
		game.field.clear();
		if (playonline) {
			onlineplayUI->show();
			gameplayUI->hide();
			challengesGameUI->hide();
			replayUI->hide();
		}
		else {
			gameplayUI->hide();
			mainMenu->show();
		}
	}
	else if (state == CountDown) {
        game.sRKey();
        game.sLKey();
        game.sDKey();
        game.gameover=false;
        game.showPressEnterText=false;
        game.draw();
	}
	else if (state == Game || state == Practice) {
		linesSent=0;
        garbageCleared=0;
        linesBlocked=0;
        gamedatacount=0;
		gamedata=sf::seconds(0);
		if (state == Practice) {
			if (!game.field.text.ready)
				ready();
			if (away)
				unAway();
			game.field.clear();
		}
		game.showPressEnterText=false;
        game.startGame();
	}
	else if (state == GameOver) {
        if (game.autoaway)
            goAway();
        if (game.sendgameover)
            sendGameOver();
        if (game.winner)
            sendGameWinner();
        if (gamestate != Replay)
        	game.showPressEnterText=true;
        else
        	replayUI->pauseTime=sf::seconds(0);
        game.field.text.setCountdown(0);
        game.draw();
	}
	else if (state == Replay) {
		game.showPressEnterText=false;
		game.startReplay();
	}

	gamestate = state;
}

void UI::disconnect() {
	net.disconnect();
	playonline=false;
	udpConfirmed=false;
	performanceOutput->ping->hide();
}

void UI::quickMsg(const sf::String& msg) {
	QuickMsg->setText(msg);
	QuickMsg->show();
	quickMsgTime = delayClock.getElapsedTime();
}

void UI::chatFocus(bool i) {
	if (i) {
		chatFocused=true;
		window->setKeyRepeatEnabled(true);
	}
	else {
		chatFocused=false;
		window->setKeyRepeatEnabled(false);
	}
}

void UI::chatMsg(const sf::String& to, const sf::String& msg) {
	if (!msg.getSize())
		return;
	if (spamCount>7000) {
		gameplayUI->Chat->get<tgui::ChatBox>(to)->addLine("I HAVE TO STOP SPAMMING THE CHAT!!!", sf::Color(235, 130, 0));
		if (to == "Lobby")
			onlineplayUI->Lobby->addLine("I HAVE TO STOP SPAMMING THE CHAT!!!", sf::Color(235, 130, 0));
		gameplayUI->ChatBox->setText("");
		onlineplayUI->ChatBox->setText("");
		spamCount=12000;
		return;
	}
	if (msg[0]=='/' && msg[1]=='w' && msg[2]==' ') {
		short until = msg.find(' ', 3);
		sf::String privto = msg.substring(3, until-3);
		sf::String privmsg = msg.substring(until, sf::String::InvalidPos);
		sendMsg(privto, privmsg);
		gameplayUI->ChatBox->setText("");
		onlineplayUI->ChatBox->setText("");
		spamCount+=2000;
		return;
	}
	sf::String postmsg = game.field.text.name + ": " + msg;
	gameplayUI->ChatBox->setText("");
	onlineplayUI->ChatBox->setText("");

	for (unsigned int i=0; i<msg.getSize(); i++)
		if (msg[i] != ' ') {
			gameplayUI->Chat->get<tgui::ChatBox>(to)->addLine(postmsg, sf::Color(235, 130, 0));
			if (to == "Lobby")
				onlineplayUI->Lobby->addLine(postmsg, sf::Color(235, 130, 0));
			sendMsg(to, msg);
			spamCount+=2000;
			return;
		}
}

void UI::sendMsg(const sf::String& to, const sf::String& msg) {
	sf::Uint8 packetid = 10;
	net.packet.clear();
	net.packet << packetid;
	if (to == "Room") {
		packetid = 1;
		net.packet << packetid << msg;
		net.sendTCP();
	}
	else if (to == "Lobby") {
		packetid = 2;
		net.packet << packetid << msg;
		net.sendTCP();
	}
	else {
		packetid = 3;
		net.packet << packetid << to << msg;
		net.sendTCP();
	}
}

void UI::getMsg() {
	sf::String name, msg;
	sf::Uint8 type;
	net.packet >> type >> name >> msg;
	if (type == 3) { // Private msg
		short create=-1;
		for (size_t i = 0; i < gameplayUI->privChats.size(); i++)
			if (gameplayUI->privChats[i].name == name) {
				create=i;
			}
		if (create == -1) {
			PrivChat newchat;
			newchat.name=name;
			gameplayUI->privChats.push_back(move(newchat));
			create=gameplayUI->privChats.size()-1;


			gameplayUI->privChats[create].chatBox = themeBB->load("ChatBox");
			gameplayUI->privChats[create].chatBox->setPosition(5, 35);
			gameplayUI->privChats[create].chatBox->setSize(480, 475);
			gameplayUI->privChats[create].chatBox->hide();
			gameplayUI->Chat->add(gameplayUI->privChats[create].chatBox, name);
			int tmp = gameplayUI->ChatTab->getSelectedIndex();
			gameplayUI->ChatTab->add(name);
			gameplayUI->ChatTab->select(tmp);
		}
		gameplayUI->privChats[create].chatBox->addLine(name + ": " + msg);
	}
	else if (type == 2) { // Lobby msg
		gameplayUI->Lobby->addLine(name + ": " + msg);
		onlineplayUI->Lobby->addLine(name + ": " + msg);
	}
	else // Room msg
		gameplayUI->Room->addLine(name + ": " + msg);
}

void UI::Chat() {
	if (gameplayUI->Chat->isVisible())
		gameplayUI->InGameTab->select(0);
	else
		gameplayUI->InGameTab->select(2);
}

void UI::Score() {
	if (gameplayUI->Score->isVisible())
		gameplayUI->InGameTab->select(0);
	else if (!chatFocused)
		gameplayUI->InGameTab->select(1);
}

void UI::receiveRecording() {
	sf::Uint16 type;
	net.packet >> type;
	game.recorder.receiveRecording(net);
	setGameState(Replay);
	if (type >= 20000) {
		gameplayUI->hide();
		challengesGameUI->show();
		challengesGameUI->showPanel(type-19994);
		replayUI->show();
	}
}

void UI::delayCheck() {
	sf::Time currentTime = delayClock.getElapsedTime();
	if (QuickMsg->isVisible())
		if (currentTime - quickMsgTime > sf::seconds(5))
			QuickMsg->hide();

	if (playonline) {
		if (!udpConfirmed)
			if (currentTime - udpPortTime > sf::milliseconds(500)) {
				udpPortTime = currentTime;
				net.sendUdpConfirm(myId);
			}
		if (currentTime - performanceOutput->pingTime > sf::seconds(1)) {
			if (!performanceOutput->pingReturned)
				performanceOutput->setPing(999);
			performanceOutput->pingTime = currentTime;
			performanceOutput->pingIdCount++;
			net.sendPing(myId, performanceOutput->pingIdCount);
		}
		if (onlineplayUI->isVisible())
			if (onlineplayUI->roomList.isVisible())
				if (currentTime - onlineplayUI->updateRoomListTime > sf::seconds(5)) {
					onlineplayUI->updateRoomListTime = currentTime;
					net.sendSignal(16);
				}
		if (challengesGameUI->isVisible() && (gamestate == Game || gamestate == Replay))
			challengesGameUI->update();
	}

	sf::Time tmp = currentTime - spamTime;
	spamCount-=tmp.asMilliseconds();
	spamTime = currentTime;
	if (spamCount<0)
		spamCount=0;
}