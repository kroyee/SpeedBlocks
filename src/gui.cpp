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
#include "GameStandings.h"
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
      away(false),
      linesSent(0),
      garbageCleared(0),
      linesBlocked(0),
      clientVersion(0),
      udpConfirmed(false),
      gamestate(game_.resources.gamestate),
      spamCount(0),
      gameFieldDrawer(*this),
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

	pos.left=330; pos.top=185; pos.width=120; pos.height=100;
	gameStandings = new GameStandings;
	gameStandings->create(pos, this);

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
	delete gameStandings;
}

void UI::joinRoom(sf::Uint16 id) {
	net.sendSignal(0, id);
	away=false;
	game.autoaway=false;
}

void UI::leaveRoom() {
	net.sendSignal(1);
	setGameState(MainMenu);
}

void UI::setGameState(GameStates state) {
	if (gamestate == MainMenu) { // Reset depending on what state we come from
		mainMenu->hide();
		onlineplayUI->hide();
		if (state != MainMenu) {
			gameplayUI->show();
			gameStandings->hide();
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
	else if (gamestate == Spectating) {
		if (state != Spectating)
			net.sendSignal(20);
	}
	else if (gamestate == Replay) {
		replayUI->playPause->setText("Play");
	}

	if (state == MainMenu) { // Set depending on what state we are going into
		gameFieldDrawer.removeAllFields();
		game.field.clear();
		away=false;
		game.autoaway=false;
		game.field.text.away=false;
		game.sendgameover=false;
		game.winner=false;
		if (playonline) {
			onlineplayUI->show();
			gameplayUI->hide();
			challengesGameUI->hide();
			replayUI->hide();
			gameStandings->hide();
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
	onlineplayUI->matchButton->setText("Join 1vs1 matchmaking");
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
		challengesGameUI->showPanel(type);
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
		if (onlineplayUI->isVisible()) {
			if (onlineplayUI->roomList.isVisible())
				if (currentTime - onlineplayUI->updateRoomListTime > sf::seconds(5)) {
					onlineplayUI->updateRoomListTime = currentTime;
					net.sendSignal(16);
				}
			if (onlineplayUI->tournamentList.isVisible())
				if (currentTime - onlineplayUI->updateTournamentListTime > sf::seconds(5)) {
					onlineplayUI->updateTournamentListTime = currentTime;
					net.sendSignal(15);
				}
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

void UI::goAway() {
	away=true;
	net.sendSignal(5);
	game.gameover=true;
	game.sendgameover=true;
	game.field.text.away=true;
	game.autoaway=false;
	game.draw();
}

void UI::unAway() {
	away=false;
	game.autoaway=false;
	net.sendSignal(6);
	game.field.text.away=false;
	game.draw();
}

void UI::ready() {
	if (gamestate != GameOver)
		return;
	if (game.field.text.ready) {
		net.sendSignal(8);
		game.field.text.ready=false;
	}
	else {
		net.sendSignal(7);
		game.field.text.ready=true;
	}
	game.draw();
}

void UI::handleEvent(sf::Event& event) {
	gameInput(event);
	windowEvents(event);
	
	if (gameOptions->SelectKey->isVisible())
		gameOptions->putKey(event);
	
	if (gameplayUI->isVisible())
		gameFieldDrawer.enlargePlayfield(event);

	keyEvents(event);

	if (onlineplayUI->isVisible()) {
		onlineplayUI->roomList.scrolled(event);
		onlineplayUI->tournamentList.scrolled(event);
		onlineplayUI->challengesUI.challengeList.scrolled(event);
		onlineplayUI->challengesUI.scrolled(event);
	}

	tGui.handleEvent(event);
}

void UI::gameInput(sf::Event& event) {
	if (gamestate != Replay && gamestate != MainMenu)
		if (event.type == sf::Event::KeyPressed) {
			if (event.key.code == options.chat)
                Chat();
            else if (event.key.code == options.score)
                Score();
            else if (event.key.code == options.away && playonline && gamestate != Spectating) {
                if (away)
                    unAway();
                else
                    goAway();
            }
		}
	if (gamestate == CountDown) {
		if (event.type == sf::Event::KeyPressed && !chatFocused) {
            if (event.key.code == options.right)
                game.rKey=true;
            else if (event.key.code == options.left)
                game.lKey=true;
        }
        else if (event.type == sf::Event::KeyReleased) {
            if (event.key.code == options.right)
                game.rKey=false;
            else if (event.key.code == options.left)
                game.lKey=false;
        }
	}
	else if (gamestate == Game || gamestate == Practice) {
		if (event.type == sf::Event::KeyPressed && !chatFocused) {
            if (event.key.code == options.right)
                game.mRKey();
            else if (event.key.code == options.left)
                game.mLKey();
            else if (event.key.code == options.rcw)
                game.rcw();
            else if (event.key.code == options.rccw)
                game.rccw();
            else if (event.key.code == options.r180)
                game.r180();
            else if (event.key.code == options.down)
                game.mDKey();
            else if (event.key.code == options.hd)
                game.hd();
        }
        else if (event.type == sf::Event::KeyReleased) {
            if (event.key.code == options.right)
                game.sRKey();
            else if (event.key.code == options.left)
                game.sLKey();
            else if (event.key.code == options.down)
                game.sDKey();
        }
	}
	else if (gamestate == GameOver) {
		if (event.type == sf::Event::KeyPressed && !chatFocused) {
            if (event.key.code == sf::Keyboard::Return && !areYouSure->isVisible()) {
            	if (playonline) {
            		game.gameover=false;
            		if (gameplayUI->isVisible())
            			setGameState(Practice);
            		else if (challengesGameUI->isVisible())
            			ready();
            	}
            	else {
	                setGameState(CountDown);
	                game.startCountdown();
	                game.gameover=false;
            	}
            }
            else if (event.key.code == options.ready && playonline)
            	ready();
        }
	}
}

void UI::windowEvents(sf::Event& event) {
	if (event.type == sf::Event::Closed)
        window->close();
    else if (event.type == sf::Event::Resized && options.currentmode == -1) {
        resizeWindow(event);
    }
}

void UI::resizeWindow(sf::Event& event) {
	sf::View view = window->getView();
    float ratio;
    if ((float)event.size.width/event.size.height > 960.0/600) {
        ratio = (event.size.height * (960.0/600)) / event.size.width;
        view.setViewport(sf::FloatRect((1-ratio)/2.0, 0, ratio, 1));
    }
    else {
        ratio = (event.size.width / (960.0/600)) / event.size.height;
        view.setViewport(sf::FloatRect(0, (1-ratio)/2.0, 1, ratio));
    }
    window->setView(view);
    tGui.setView(view);
    window->clear(sf::Color(0,0,0));
}

void UI::keyEvents(sf::Event& event) {
	if (event.type == sf::Event::KeyPressed) {
		if (event.key.code == sf::Keyboard::Escape) {
			if (loginBox->isVisible()) {
				loginBox->closeLogin();
			}
			else if (mainMenu->isVisible()) {
				if (areYouSure->isVisible()) {
					areYouSure->hide();
					mainMenu->enable();
				}
				else {
					areYouSure->label->setText("Do you want to quit?");
					areYouSure->show();
					mainMenu->disable();
				}
			}
			else if (gameOptions->isVisible())
				gameOptions->otab->select("Back");
			else if (onlineplayUI->isVisible())
				onlineplayUI->opTab->select("Back");
			else if (gameplayUI->isVisible()) {
				if (areYouSure->isVisible())
					areYouSure->hide();
				else {
					areYouSure->label->setText("Leave this game?");
					areYouSure->show();
				}
			}
			else if (challengesGameUI->isVisible()) {
				if (areYouSure->isVisible())
					areYouSure->hide();
				else {
					areYouSure->label->setText("Leave this challenge?");
					areYouSure->show();
				}
			}
		}
		else if (event.key.code == sf::Keyboard::Return)
			if (areYouSure->isVisible())
				areYouSure->ausY();
	}
}

void UI::sendGameData() {
	sf::Time tmp = game.gameclock.getElapsedTime();
	if (tmp>gamedata) {
		gamedata+=sf::milliseconds(100);
		if (gamedata < tmp)
			gamedata=tmp+sf::milliseconds(100);
		sendGameState();
	}

	if (game.linesSent > linesSent) {
		sf::Uint8 amount = game.linesSent-linesSent;
		net.sendSignal(2, amount);
		linesSent = game.linesSent;
	}

	if (game.garbageCleared > garbageCleared) {
		sf::Uint8 amount = game.garbageCleared-garbageCleared;
		net.sendSignal(3, amount);
		garbageCleared = game.garbageCleared;
	}

	if (game.linesBlocked > linesBlocked) {
		sf::Uint8 amount = game.linesBlocked-linesBlocked;
		net.sendSignal(4, amount);
		linesBlocked = game.linesBlocked;
	}
}

void UI::sendGameState() {
	if (gamestate == CountDown) {
		sf::Uint8 tmp = game.field.piece.piece;
		game.field.piece.piece = 7; // makes the current piece not draw on other players screen (since it's countdown)
		compressor.compress();
		game.field.piece.piece = tmp;
	}
	else
		compressor.compress();
	net.packet.clear();
	sf::Uint8 packetid = 100;
	net.packet << packetid << myId << gamedatacount;
	gamedatacount++;
	for (int i=0; i<compressor.tmpcount; i++)
		net.packet << compressor.tmp[i];
	if (compressor.bitcount>0)
		net.packet << compressor.tmp[compressor.tmpcount];
	net.sendUDP();
}

void UI::sendGameOver() {
	sf::Uint8 packetid = 3;
	net.packet.clear();
	net.packet << packetid << game.maxCombo << game.linesSent << game.linesRecieved << game.linesBlocked << game.bpm << game.linesPerMinute;
	net.sendTCP();
	game.sendgameover=false;

	sendGameState();
}

void UI::sendGameWinner() {
	sf::Uint8 packetid = 4;
	net.packet.clear();
	net.packet << packetid << game.maxCombo << game.linesSent << game.linesRecieved << game.linesBlocked;
	net.packet << game.bpm << game.linesPerMinute << (sf::Uint32)game.recorder.duration.asMilliseconds();
	net.packet << (sf::Uint16)game.pieceCount;
	net.sendTCP();
	game.winner=false;
}

void UI::iGotKicked(sf::Uint16 reason) {
	if (reason == 1)
		quickMsg("Kicked: game Clock out of sync");
	else
		quickMsg("Kicked from room for unknown reason");
	setGameState(MainMenu);
}

void UI::handlePacket() {
	if (net.packetid <100)
		cout << "Packet id: " << (int)net.packetid << endl;
	switch ((int)net.packetid) {
		case 255: //Disconnected from server
			disconnect();
			onlineplayUI->hide();
			areYouSure->hide();
			connectingScreen->hide();
			mainMenu->enable();
			loginBox->hide();
			challengesGameUI->hide();
			replayUI->hide();
			playonline=false;
			performanceOutput->ping->hide();
			setGameState(MainMenu);
			quickMsg("Disconnected from server");
		break;
		case 100: //Game data
		{
			sf::Uint16 clientid;
			sf::Uint8 datacount;
			net.packet >> clientid >> datacount;
			for (auto&& field : gameFieldDrawer.fields)
				if (field.id==clientid) {
					if (datacount>field.datacount || (datacount<50 && field.datacount>200)) {
						field.datacount=datacount;
						for (int c=0; net.packet >> compressor.tmp[c]; c++) {}
						compressor.extract();
						if (compressor.validate()) {
							compressor.field = &field;
							compressor.copy();
							gameFieldDrawer.drawOppField(field);
						}
					}
					break;
				}
		}
		break;
		case 0: //Welcome packet
		{
			sf::String welcomeMsg;

			net.packet >> myId >> welcomeMsg;

			gameplayUI->Lobby->addLine("Server: " + welcomeMsg);
			onlineplayUI->Lobby->addLine("Server: " + welcomeMsg);

			onlineplayUI->makeRoomList();
			onlineplayUI->makeClientList();
			performanceOutput->ping->show();
		}
		break;
		case 1:
			receiveRecording();
		break;
		case 2:
			onlineplayUI->challengesUI.makeList();
		break;
		case 3://Join room ok/no
		{
			sf::Uint16 joinok;
			net.packet >> joinok;
			if (joinok == 1 || joinok == 1000) {
				sf::Uint8 playersinroom;
				sf::Uint16 playerid, seed1, seed2;
				net.packet >> seed1 >> seed2 >> playersinroom;
				game.rander.seedPiece(seed1);
				game.rander.seedHole(seed2);
				game.rander.reset();

				if (joinok == 1) {
					setGameState(GameOver);
					game.pressEnterText.setString("press Enter to start practice");
					game.field.clear();
					game.draw();
					gameFieldDrawer.setPosition(465, 40);
					gameFieldDrawer.setSize(490, 555);
				}
				else {
					setGameState(Spectating);
					gameFieldDrawer.setPosition(5, 40);
					gameFieldDrawer.setSize(950, 555);
				}

				obsField newfield(resources);
				newfield.clear();
				sf::String name;
				for (int c=0; c<playersinroom; c++) {
					net.packet >> playerid >> name;
					newfield.id = playerid;
					gameFieldDrawer.addField(newfield);
					gameFieldDrawer.fields.back().text.setName(name);
					gameFieldDrawer.fields.back().drawField();
				}
				if (gamestate == Spectating)
					gameStandings->alignResult();
			}
			else if (joinok == 2)
				quickMsg("Room is full");
			else if (joinok == 3)
				quickMsg("Please wait for server to get your user-data");
			else if (joinok == 4)
				quickMsg("This is not your game");
			else if (joinok >= 20000) {
				setGameState(GameOver);
				gameplayUI->hide();
				game.pressEnterText.setString("press Enter to start challenge");
				game.field.clear();
				game.draw();
				challengesGameUI->show();
				challengesGameUI->showPanel(joinok);
				challengesGameUI->clear();
			}
		}
		break;
		case 4: //Another player joined room
		{
			sf::String name;
			obsField newfield(resources);
			newfield.clear();
			net.packet >> newfield.id >> name;
			gameFieldDrawer.addField(newfield);
			gameFieldDrawer.fields.back().text.setName(name);
			gameFieldDrawer.fields.back().drawField();
			if (gameStandings->isVisible())
				gameStandings->alignResult();
		}
		break;
		case 5:
			onlineplayUI->challengesUI.makeLeaderboard();
		break;
		case 6:
		{
			sf::String text;
			net.packet >> text;
			quickMsg("You improved your score from " + text);
			game.recorder.sendRecording(net, onlineplayUI->challengesUI.selectedId);
		}
		break;
		case 7:
		{
			sf::String text;
			net.packet >> text;
			quickMsg("Your score of " + text);
		}
		case 8: // Round score data
		{
			sf::Uint8 count;
			net.packet >> count;
			gameplayUI->clearScore();
			for (int i=0; i<count; i++)
				gameplayUI->scoreRow();
		}
		break;
		case 9: // Auth result
		{
			sf::Uint8 success;
			net.packet >> success;
			if (success == 1) {
				sf::String name;
				net.packet >> name >> myId;
				game.field.text.setName(name);
				connectingScreen->hide();
				onlineplayUI->show();
				onlineplayUI->opTab->select(0);
				mainMenu->enable();
			}
			else if (success == 2) {
				connectingScreen->hide();
				onlineplayUI->show();
				onlineplayUI->opTab->select(0);
				mainMenu->enable();
			}
			else {
				disconnect();
				if (success == 3)
					quickMsg("You have the wrong client version, check https://speedblocks.se for updates");
				else if (success == 4)
					quickMsg("Name already in use");
				else
					quickMsg("Authentication failed");
				connectingScreen->hide();
				mainMenu->show();
				loginBox->show();
				performanceOutput->ping->hide();
			}
		}
		break;
		case 12: // Incoming chat msg
			getMsg();
		break;
		case 16: // Server sending room list
			onlineplayUI->makeRoomList();
		break;
		case 17: // New room created
			onlineplayUI->addRoom();
		break;
		case 18: // Room was removed
		{
			sf::Uint16 id;
			net.packet >> id;
			onlineplayUI->roomList.removeItem(id);
		}
		break;
		case 20: // Another client connected to the server
			onlineplayUI->addClient();
		break;
		case 21: // Another client left the server
			onlineplayUI->removeClient();
		break;
		case 22: // Get tournament list
			onlineplayUI->makeTournamentList();
		break;
		case 23: // Get tournament info
			onlineplayUI->tournamentPanel.getInfo(myId);
		break;
		case 24: // Get tournament game standings
			gameStandings->setResults();
		break;
		case 27: // Tournament update
			onlineplayUI->tournamentPanel.getUpdate(myId);
		break;
		case 102: // Ping packet returned from server
		{
			sf::Uint8 pingId;
			sf::Uint16 clientid;
			net.packet >> clientid >> pingId;
			if (pingId == performanceOutput->pingIdCount) {
				net.sendUDP();
				sf::Time pingResult = delayClock.getElapsedTime() - performanceOutput->pingTime;
				performanceOutput->setPing(pingResult.asMilliseconds());
				performanceOutput->pingReturned=true;
			}
		}
		break;
		case 254: // Signal packet
			handleSignal();
		break;
	}
}

void UI::handleSignal() {
	sf::Uint8 signalId;
	sf::Uint16 id1, id2;

	net.packet >> signalId;
	if (!net.packet.endOfPacket())
		net.packet >> id1;
	if (!net.packet.endOfPacket())
		net.packet >> id2;

	//cout << "Signal id: " << (int)signalId << endl;

	switch (signalId) {
		case 0:
			quickMsg("Not enough players to start tournament");
		break;
		case 1: // A tournament game is ready
			onlineplayUI->alertMsg(id1);
		break;
		case 2: // Not allowed if logged in as guest
			quickMsg("You can't do that as guest, register at https://speedblocks.se");
		break;
		case 3: // Update on waiting time until WO in tournament game
			gameStandings->setWaitTime(id1);
		break;
		case 4: //Start countdown
			game.rander.seedPiece(id1);
			game.rander.seedHole(id2);
			game.rander.reset();
			game.startCountdown();
			game.countDown(3);
			gameFieldDrawer.resetOppFields();
			setGameState(CountDown);
			gamedatacount=251;
			sendGameState();
			if (challengesGameUI->isVisible()) {
				challengesGameUI->clear();
				if (challengesGameUI->cheesePanel->isVisible()) {
					int lastHole=10, nextHole=10;
					for (int i=0; i<9; i++) {
						while (nextHole == lastHole)
							nextHole = game.rander.getHole();
						game.addGarbageLine(nextHole);
						lastHole=nextHole;
					}
					game.draw();
				}
			}
		break;
		case 5: //Countdown ongoing
			if (gamestate != CountDown)
				setGameState(CountDown);
			if (!game.rander.total)
				game.startCountdown();
			game.countDown(id1);
			if (!id1)
				setGameState(Game);
			else {
				gamedatacount=255-id1;
				sendGameState();
			}
		break;
		case 6: //Another player left the room
			gameFieldDrawer.removeField(id1);
			if (gameStandings->isVisible())
				gameStandings->alignResult();
		break;
		case 7: //Round ended
			if (gamestate != Practice)
				game.gameover=true;
		break;
		case 8: //Round ended and you where the winner
			game.gameover=true;
			game.winner=true;
			game.autoaway=false;
		break;
		case 9: //Garbage received
			game.addGarbage(id1);
		break;
		case 10: //Server telling me to reset my oppfields. This is the same as Packet 1, but when client is away.
			gameFieldDrawer.resetOppFields();
			game.rander.seedPiece(id1);
			game.rander.seedHole(id2);
			game.rander.reset();
			if (gamestate != Spectating) {
				game.field.clear();
				game.draw();
			}
		break;
		case 11: // Another player went away
			for (auto&& field : gameFieldDrawer.fields)
				if (field.id == id1) {
					field.text.away=true;
					gameFieldDrawer.drawOppField(field);
				}
		break; // Another player came back
		case 12:
			for (auto&& field : gameFieldDrawer.fields)
				if (field.id == id1) {
					field.text.away=false;
					gameFieldDrawer.drawOppField(field);
				}
		break;
		case 13: // Server reported the position of a player
			for (auto&& field : gameFieldDrawer.fields)
				if (field.id == id1) {
					field.text.setPosition(id2);
					gameFieldDrawer.drawOppField(field);
				}
			if (id1 == myId) {
				game.field.text.setPosition(id2);
				game.draw();
			}
		break;
		case 14: // UDP-port was established by server
			udpConfirmed=true;
		break;
		case 15: // Players is ready
			for (auto&& field : gameFieldDrawer.fields)
				if (field.id == id1) {
					field.text.ready=true;
					field.drawField();
				}
		break;
		case 16: // Player is not ready
			for (auto&& field : gameFieldDrawer.fields)
				if (field.id == id1) {
					field.text.ready=false;
					field.drawField();
				}
		break;
		case 17: // Got kicked from room
			iGotKicked(id1);
		break;
		case 18: // Can't join Hero room if not rank 0
			quickMsg("You need to reach rank 0 to join the Hero room");
		break;
		case 19: // Joined matchmaking
			onlineplayUI->matchButton->setText("Leave 1vs1 matchmaking");
		break;
		case 20: // Left matchmaking
			onlineplayUI->matchButton->setText("Join 1vs1 matchmaking");
		break;
		case 21: // You left your match game and was removed from queue
			quickMsg("You were removed from the matchmaking queue");
			onlineplayUI->matchButton->setText("Join 1vs1 matchmaking");
		break;
		case 22: // Your left your match and are still in the queue
			quickMsg("You are still in the matchmaking queue");
		break;
	}
}