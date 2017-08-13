#include "gui.h"
#include "sounds.h"
#include "optionSet.h"
#include "gamePlay.h"
#include "network.h"
#include "MainMenu.h"
#include "LoginBox.h"
#include "GameOptions.h"
#include "Connecting.h"
#include "OnlineplayUI.h"
#include "AreYouSure.h"
#include "PerformanceOutput.h"
#include "BugReport.h"
#include "ChallengesGameUI.h"
#include "ReplayUI.h"
#include "GameStandings.h"
#include "ChatScreen.h"
#include "SlideMenu.h"
#include "ScoreScreen.h"
#include "AnimatedBackground.h"
#include "AlertsUI.h"
#include "ServerUI.h"
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
      gameFieldDrawer(*this),
      gamedata(sf::seconds(0)),
      gamedatacount(0),
      myId(0) {

    tGui.setFont(resources.gfx.standard);

    compressor.game=&game;

    themeTG = tgui::Theme::create(resourcePath() + "media/TransparentGrey.txt");

	animatedBackground = new AnimatedBackground(resources, 7);

	sf::Rect<int> pos(0,0,960,600);
	mainMenu = new Menu;
	mainMenu->create(pos, this);
	mainMenu->show();

	pos.left=500; pos.top=50; pos.width=450; pos.height=550;
	loginBox = new LoginBox;
	loginBox->create(pos, this, mainMenu->panel);
	loginBox->show();

	pos.left=0; pos.top=0; pos.width=960; pos.height=600;
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

	pos.left=330; pos.top=250; pos.width=300; pos.height=100;
	connectingScreen = new Connecting;
	connectingScreen->create(pos, this);

	pos.left=807; pos.top=0; pos.width=113; pos.height=28;
	performanceOutput = new PerformanceOutput;
	performanceOutput->create(pos, this);
	if (options.performanceOutput)
		performanceOutput->show();

	QuickMsg = tgui::Label::create();
	QuickMsg->setPosition(0,10);
	QuickMsg->setSize(960,90);
	QuickMsg->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Center);
	QuickMsg->setTextSize(32);
	QuickMsg->setTextColor(sf::Color::Red);
	QuickMsg->hide();
	QuickMsg->disable(false);
	tGui.add(QuickMsg);

	pos.left=440; pos.top=0; pos.width=480; pos.height=600;
	chatScreen = new ChatScreen(pos, this);

	pos.left=920; pos.top=0; pos.width=600; pos.height=600;
	slideMenu = new SlideMenu(pos, this);
	slideMenu->show();

	gameOptions = new GameOptions;
	pos.left=40; pos.top=40; pos.width=560; pos.height=560;
	gameOptions->create(pos, this, slideMenu->panel);

	bugReport = new BugReport;
	bugReport->create(pos, this, slideMenu->panel);

	serverUI = new ServerUI;
	serverUI->create(pos, this, slideMenu->panel);

	alertsUI = new AlertsUI;
	alertsUI->create(pos, this, slideMenu->panel);
	alertsUI->show();

	pos.left=30; pos.top=30; pos.width=860; pos.height=540;
	scoreScreen = new ScoreScreen(pos, this);

	pos.left=0; pos.top=0; pos.width=960; pos.height=600;
	areYouSure = new AreYouSure;
	areYouSure->create(pos, this);

	if (options.theme == 1)
		lightTheme();
	else
		darkTheme();

	if (!options.animatedBackground)
		animatedBackground->disable();

	setOnChatFocus(tGui.getWidgets());
}

UI::~UI() {
	delete bugReport;
	delete areYouSure;
	delete performanceOutput;
	delete connectingScreen;
	delete loginBox;
	delete onlineplayUI;
	delete gameOptions;
	delete mainMenu;
	delete challengesGameUI;
	delete replayUI;
	delete gameStandings;
	delete chatScreen;
	delete slideMenu;
	delete scoreScreen;
	delete animatedBackground;
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
			gameFieldDrawer.show();
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
		scoreScreen->hide();
		chatScreen->sendTo("Lobby");
		if (playonline) {
			onlineplayUI->show();
			gameFieldDrawer.hide();
			challengesGameUI->hide();
			replayUI->hide();
			gameStandings->hide();
		}
		else {
			gameFieldDrawer.hide();
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
	serverUI->clear();
	chatScreen->hide();
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

void UI::receiveRecording() {
	sf::Uint16 type;
	net.packet >> type;
	game.recorder.receiveRecording(net);
	setGameState(Replay);
	if (type >= 20000) {
		gameFieldDrawer.hide();
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
		performanceOutput->setPing(ping.send(currentTime, net, myId));

		if (gamestate == CountDown) {
			sf::Uint8 cd = countdown.check(currentTime);
			if (cd != 255) {
				if (game.countDown(cd))
					setGameState(Game);
				gamedatacount=255-cd;
				sendGameState();
			}
		}
	}

	slideMenu->update(currentTime);

	if (bugReport->join)
		if (bugReport->t.joinable())
			bugReport->t.join();

	chatScreen->fade(currentTime);
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
	if (gameOptions->SelectKey->isVisible())
		if (gameOptions->putKey(event))
			return;

	if (keyEvents(event))
		return;

	gameInput(event);
	windowEvents(event);
	
	if (gameFieldDrawer.isVisible())
		gameFieldDrawer.enlargePlayfield(event);

	if (onlineplayUI->isVisible()) {
		onlineplayUI->roomList.scrolled(event);
		onlineplayUI->tournamentList.scrolled(event);
		onlineplayUI->challengesUI.challengeList.scrolled(event);
		onlineplayUI->challengesUI.scrolled(event);
	}

	slideMenu->handleEvent(event);

	tGui.handleEvent(event);
}

void UI::gameInput(sf::Event& event) {
	if (gamestate != Replay && gamestate != MainMenu) {
		if (event.type == sf::Event::KeyPressed) {
            if (event.key.code == options.score)
                scoreScreen->show();
            else if (event.key.code == options.away && playonline && gamestate != Spectating) {
                if (away)
                    unAway();
                else
                    goAway();
            }
		}
		else if (event.type == sf::Event::KeyReleased)
			if (event.key.code == options.score)
				scoreScreen->hide();
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
            if (event.key.code == sf::Keyboard::P && !areYouSure->isVisible()) {
            	if (playonline) {
            		if (challengesGameUI->isVisible())
            			ready();
            		else
            			setGameState(Practice);
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
    else if (event.type == sf::Event::Resized && !options.fullscreen) {
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

bool UI::keyEvents(sf::Event& event) {
	if (event.type == sf::Event::KeyPressed) {
		if (chatScreen->isActive())
			chatScreen->focus();
		if (event.key.code == sf::Keyboard::Escape) {
			if (areYouSure->isVisible())
				areYouSure->hide();
			else if (chatScreen->isActive())
				chatScreen->deactivate();
			else if (mainMenu->isVisible()) {
				areYouSure->label->setText("Do you want to quit?");
				areYouSure->show();
			}
			else if (onlineplayUI->isVisible()) {
				areYouSure->label->setText("Leave the server?");
				areYouSure->show();
			}
			else if (challengesGameUI->isVisible()) {
				areYouSure->label->setText("Leave this challenge?");
				areYouSure->show();
			}
			else if (gamestate != MainMenu) {
				areYouSure->label->setText("Leave this game?");
				areYouSure->show();
			}
		}
		else if (event.key.code == sf::Keyboard::Return) {
			if (event.key.alt)
				toggleFullscreen();
			else if (areYouSure->isVisible())
				areYouSure->ausY();
			else if (playonline && !chatScreen->isActive()) {
				chatScreen->activate();
				return true;
			}
		}
		else if (event.key.code == sf::Keyboard::Tab && chatScreen->isActive()) {
			chatScreen->toggleSendTo();
			return true;
		}
	}
	return false;
}

void UI::toggleFullscreen() {
	if (!options.fullscreen) {
		if (options.currentmode == -1)
			options.currentmode = 0;
		window->close();
		window->create(options.modes[options.currentmode], "SpeedBlocks", sf::Style::Fullscreen);
		window->setView(sf::View(sf::FloatRect(0, 0, 960, 600)));
		options.fullscreen=true;
	}
	else if (options.fullscreen || !window->isOpen()) {
		window->close();
		window->create(sf::VideoMode(960, 600), "SpeedBlocks");
		window->setView(sf::View(sf::FloatRect(0, 0, 960, 600)));
		options.fullscreen=false;
	}
}

void UI::darkTheme() {
	animatedBackground->dark();
	slideMenu->dark();
	chatScreen->dark();
	game.field.text.setColor(sf::Color(255,255,255));
	game.pressEnterText.setFillColor(sf::Color(255,255,255));
	game.pressEnterText.setOutlineColor(sf::Color(255,255,255));
	game.draw();
	setWidgetTextColor(sf::Color(255,255,255,200));
	themeTG->setProperty("Panel", "BackgroundColor", sf::Color(25,25,25,200));
	options.theme=2;
	for (auto& field : gameFieldDrawer.fields)
		field.text.setColor(sf::Color(255,255,255));
}

void UI::lightTheme() {
	animatedBackground->light();
	slideMenu->light();
	chatScreen->light();
	game.field.text.setColor(sf::Color(0,0,0));
	game.pressEnterText.setFillColor(sf::Color(0,0,0));
	game.pressEnterText.setOutlineColor(sf::Color(0,0,0));
	game.draw();
	setWidgetTextColor(sf::Color(0,0,0,200));
	themeTG->setProperty("Panel", "BackgroundColor", sf::Color(230,230,230,200));
	options.theme=1;
	for (auto& field : gameFieldDrawer.fields)
		field.text.setColor(sf::Color(0,0,0));
}

void UI::setWidgetTextColor(sf::Color color) {
	themeTG->setProperty("Label", "TextColor", color);
	
	color.a = 215; themeTG->setProperty("Button", "TextColorNormal", color);
	color.a = 235; themeTG->setProperty("Button", "TextColorHover", color);
	themeTG->setProperty("Button", "TextColorDown", color);

	color.a = 215; themeTG->setProperty("CheckBox", "TextColorNormal", color);
	color.a = 235; themeTG->setProperty("CheckBox", "TextColorHover", color);
	color.a = 180; themeTG->setProperty("CheckBox", "CheckColorNormal", color);
	color.a = 200; themeTG->setProperty("CheckBox", "CheckColorHover", color);

	color.a = 180; themeTG->setProperty("EditBox", "TextColor", color);
	color.a = 225; themeTG->setProperty("EditBox", "SelectedTextColor", color);
	color.a = 180; themeTG->setProperty("EditBox", "DefaultTextColor", color);
	color.a = 215; themeTG->setProperty("EditBox", "CaretColor", color);

	color.a = 245; themeTG->setProperty("ListBox", "SelectedTextColor", color);
	color.a = 215; themeTG->setProperty("ListBox", "TextColorNormal", color);
	color.a = 235; themeTG->setProperty("ListBox", "TextColorHover", color);

	color.a = 215; themeTG->setProperty("Tab", "TextColor", color);
	color.a = 245; themeTG->setProperty("Tab", "SelectedTextColor", color);

	color.a = 215; themeTG->setProperty("RadioButton", "TextColorNormal", color);
	color.a = 235; themeTG->setProperty("RadioButton", "TextColorHover", color);
	color.a = 180; themeTG->setProperty("RadioButton", "CheckColorNormal", color);
	color.a = 200; themeTG->setProperty("RadioButton", "CheckColorHover", color);

	color.a = 180; themeTG->setProperty("TextBox", "TextColor", color);
	color.a = 225; themeTG->setProperty("TextBox", "SelectedTextColor", color);
	color.a = 215; themeTG->setProperty("TextBox", "CaretColor", color);
}

void UI::setOnChatFocus(const std::vector<tgui::Widget::Ptr> widgets) {
	for (auto& widget : widgets) {
		if (widget->getWidgetType() == "Panel")
			setOnChatFocus(std::static_pointer_cast<tgui::Panel>(widget)->getWidgets());
		else if (widget->getWidgetType() == "EditBox" || widget->getWidgetType() == "TextBox")
			widget->connect("Focused Unfocused", &UI::chatFocus, this, std::bind(&tgui::Widget::isFocused, widget));
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

	if (game.garbage.linesBlocked > linesBlocked) {
		sf::Uint8 amount = game.garbage.linesBlocked-linesBlocked;
		net.sendSignal(4, amount);
		linesBlocked = game.garbage.linesBlocked;
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
	net.packet << packetid << game.combo.maxCombo << game.linesSent << game.linesRecieved << game.garbage.linesBlocked << game.bpm;
	net.sendTCP();
	game.sendgameover=false;

	sendGameState();
}

void UI::sendGameWinner() {
	sf::Uint8 packetid = 4;
	net.packet.clear();
	net.packet << packetid << game.combo.maxCombo << game.linesSent << game.linesRecieved << game.garbage.linesBlocked;
	net.packet << game.bpm << (sf::Uint32)game.recorder.duration.asMilliseconds();
	net.packet << (sf::Uint16)game.pieceCount;
	net.sendTCP();
	game.winner=false;

	sendGameState();
}

void UI::iGotKicked(sf::Uint16 reason) {
	if (reason == 1)
		quickMsg("Kicked: game Clock out of sync");
	else
		quickMsg("Kicked from room for unknown reason");
	setGameState(MainMenu);
}

void UI::setCountdown() {
	countdown.set(delayClock.getElapsedTime(), net.packet);
	if (countdown.ongoing() && !away && gamestate != CountDown) {
		setGameState(CountDown);
		game.startCountdown();
	}
}

void UI::getAlert() {
	sf::String msg;
	net.packet >> msg;
	alertsUI->addAlert(msg);
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

			serverUI->motd->setText(welcomeMsg);

			onlineplayUI->makeRoomList();
			serverUI->makeClientList();
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
					game.pressEnterText.setString("press P to start practice");
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

				chatScreen->sendTo("Room");
			}
			else if (joinok == 2)
				quickMsg("Room is full");
			else if (joinok == 3)
				quickMsg("Please wait for server to get your user-data");
			else if (joinok == 4)
				quickMsg("This is not your game");
			else if (joinok >= 20000) {
				setGameState(GameOver);
				gameFieldDrawer.hide();
				game.pressEnterText.setString("press P to start challenge");
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
			scoreScreen->getScores(net.packet);
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
				serverUI->putClient(myId, name);
			}
			else if (success == 2) {
				connectingScreen->hide();
				onlineplayUI->show();
				onlineplayUI->opTab->select(0);
				serverUI->putClient(myId, game.field.text.name);
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
		case 10: //Alert msg
			getAlert();
		break;
		case 12: // Incoming chat msg
			chatScreen->getMsg();
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
			serverUI->addClient();
		break;
		case 21: // Another client left the server
			serverUI->removeClient();
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
			performanceOutput->setPing(ping.get(delayClock.getElapsedTime(), net));
		break;
		case 103: // Countdown packet with time elapsed from countdown start
			setCountdown();
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
			countdown.start(delayClock.getElapsedTime());
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
		case 5: //Countdown ended
			if (!id1 && gamestate==Game)
				game.gameover=true;
			countdown.stop();
		break;
		case 6: //Another player left the room
			gameFieldDrawer.removeField(id1);
			if (gameStandings->isVisible())
				gameStandings->alignResult();
		break;
		case 7: //Round ended
			if (gamestate != Practice)
				game.gameover=true;
			countdown.stop();
		break;
		case 8: //Round ended and you where the winner
			game.gameover=true;
			game.winner=true;
			game.autoaway=false;
			countdown.stop();
		break;
		case 9: //Garbage received
			game.addGarbage(id1);
		break;
		case 10: //Server telling me to reset my oppfields. This is the same as Signal 4, but when client is away.
			gameFieldDrawer.resetOppFields();
			game.rander.seedPiece(id1);
			game.rander.seedHole(id2);
			game.rander.reset();
			if (gamestate != Spectating) {
				game.field.clear();
				game.draw();
			}
			countdown.start(delayClock.getElapsedTime());
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