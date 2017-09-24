#include "gui.h"
#include "optionSet.h"
#include "gamePlay.h"
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
#include "textures.h"
#include "Signal.h"
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
    : resources(game_.resources),
      options(*game_.resources.options),
      game(game_),
      net(*game_.resources.net),
      window(&window_),
      chatFocused(false),
      away(false),
      linesSent(0),
      garbageCleared(0),
      linesBlocked(0),
      delayClock(resources.delayClock),
      udpConfirmed(false),
      gamestate(game_.resources.gamestate),
      gameFieldDrawer(resources),
      gamedata(sf::seconds(0)),
      gamedatacount(0),
      myId(resources.myId) {

    compressor.game=&game;

	animatedBackground = new AnimatedBackground(resources, 7);

	sf::Rect<int> pos(0,0,960,600);
	mainMenu = new Menu(pos, resources);
	mainMenu->show();

	pos.left=500; pos.top=50; pos.width=450; pos.height=550;
	loginBox = new LoginBox(pos, resources, mainMenu->panel);
	loginBox->show();

	pos.left=0; pos.top=0; pos.width=960; pos.height=600;
	challengesGameUI = new ChallengesGameUI(pos, resources);
	onlineplayUI = new OnlineplayUI(pos, resources);

	pos.left=330; pos.top=185; pos.width=120; pos.height=100;
	gameStandings = new GameStandings(pos, resources);

	pos.left=465; pos.top=530; pos.width=490; pos.height=70;
	replayUI = new ReplayUI(pos, resources);

	pos.left=807; pos.top=0; pos.width=113; pos.height=28;
	performanceOutput = new PerformanceOutput(pos, resources);
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
	resources.gfx->tGui.add(QuickMsg);

	pos.left=440; pos.top=0; pos.width=480; pos.height=600;
	chatScreen = new ChatScreen(pos, resources);

	pos.left=920; pos.top=0; pos.width=600; pos.height=600;
	slideMenu = new SlideMenu(pos, resources);
	slideMenu->show();

	pos.left=40; pos.top=40; pos.width=560; pos.height=560;
	gameOptions = new GameOptions(pos, resources, slideMenu->panel);

	bugReport = new BugReport(pos, resources, slideMenu->panel);

	serverUI = new ServerUI(pos, resources, slideMenu->panel);

	alertsUI = new AlertsUI(pos, resources, slideMenu->panel);
	alertsUI->show();

	pos.left=30; pos.top=30; pos.width=860; pos.height=540;
	scoreScreen = new ScoreScreen(pos, resources);

	pos.left=0; pos.top=0; pos.width=960; pos.height=600;
	areYouSure = new AreYouSure(pos, resources);

	guiElements.push_back(mainMenu);
	guiElements.push_back(loginBox);
	guiElements.push_back(gameOptions);
	guiElements.push_back(&loginBox->connectingScreen);
	guiElements.push_back(onlineplayUI);
	guiElements.push_back(areYouSure);
	guiElements.push_back(performanceOutput);
	guiElements.push_back(bugReport);
	guiElements.push_back(challengesGameUI);
	guiElements.push_back(replayUI);
	guiElements.push_back(gameStandings);
	guiElements.push_back(chatScreen);
	guiElements.push_back(slideMenu);
	guiElements.push_back(scoreScreen);
	guiElements.push_back(serverUI);
	guiElements.push_back(alertsUI);

	Signals::Show.connect(&UI::showElement, this);
	Signals::Hide.connect(&UI::hideElement, this);
	Signals::Enable.connect(&UI::enableElement, this);
	Signals::Disable.connect(&UI::disableElement, this);
	Signals::IsVisible.connect(&UI::isVisible, this);
	Signals::LeaveRoom.connect(&UI::leaveRoom, this);
	Signals::Disconnect.connect(&UI::disconnect, this);
	Signals::SetGameState.connect(&UI::setGameState, this);
	Signals::QuickMsg.connect(&UI::quickMsg, this);
	Signals::LightTheme.connect(&UI::lightTheme, this);
	Signals::DarkTheme.connect(&UI::darkTheme, this);
	Signals::JoinRoom.connect(&UI::joinRoom, this);

	Net::takePacket(100, &UI::getGameState, this);
	Net::takePacket(0, [&](sf::Packet& packet){
		sf::String welcomeMsg;
		packet >> myId >> welcomeMsg;
		serverUI->motd->setText(welcomeMsg);
		onlineplayUI->makeRoomList(packet);
		serverUI->makeClientList(packet);
		performanceOutput->ping->show();
	});
	Net::takePacket(1, &UI::receiveRecording, this);
	Net::takePacket(3, &UI::joinRoomResponse, this);
	Net::takePacket(4, [&](sf::Packet &packet){
		sf::String name;
		obsField newfield(resources);
		newfield.clear();
		packet >> newfield.id >> name;
		gameFieldDrawer.addField(newfield);
		gameFieldDrawer.fields.back().text.setName(name);
		gameFieldDrawer.fields.back().drawField();
		if (gameStandings->isVisible())
			gameStandings->alignResult();
	});
	Net::takePacket(6, [&](sf::Packet &packet){
		sf::String text;
		packet >> text;
		quickMsg("You improved your score from " + text);
		game.recorder.sendRecording(onlineplayUI->challengesUI.selectedId);
	});
	Net::takePacket(7, [&](sf::Packet &packet){
		sf::String text;
		packet >> text;
		quickMsg("Your score of " + text);
	});
	Net::takePacket(9, &UI::getAuthResult, this);
	Net::takePacket(102, [&](sf::Packet &packet){
		performanceOutput->setPing(ping.get(delayClock.getElapsedTime(), packet));
	});
	Net::takePacket(103, &UI::setCountdown, this);

	Net::takeSignal(0, [&](){ quickMsg("Not enough players to start tournament"); });
	Net::takeSignal(2, [&](){ quickMsg("You can't do that as guest, register at https://speedblocks.se"); });
	Net::takeSignal(4, [&](sf::Uint16 id1, sf::Uint16 id2){
		game.rander.seedPiece(id1);
		game.rander.seedHole(id2);
		game.rander.reset();
		game.startCountdown();
		game.countDown(3);
		gameFieldDrawer.resetOppFields();
		setGameState(GameStates::CountDown);
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
	});
	Net::takeSignal(5, [&](sf::Uint16 id1){
		if (!id1 && gamestate==GameStates::Game)
			game.gameover=true;
		countdown.stop();
	});
	Net::takeSignal(6, [&](sf::Uint16 id1){
		gameFieldDrawer.removeField(id1);
		if (gameStandings->isVisible())
			gameStandings->alignResult();
	});
	Net::takeSignal(7, [&](){
		if (gamestate != GameStates::Practice)
			game.gameover=true;
		countdown.stop();
	});
	Net::takeSignal(8, [&](){
		game.gameover=true;
		game.winner=true;
		game.autoaway=false;
		countdown.stop();
	});
	Net::takeSignal(10, [&](sf::Uint16 id1, sf::Uint16 id2){
		gameFieldDrawer.resetOppFields();
		game.rander.seedPiece(id1);
		game.rander.seedHole(id2);
		game.rander.reset();
		if (gamestate != GameStates::Spectating) {
			game.field.clear();
			game.draw();
		}
		countdown.start(delayClock.getElapsedTime());
	});
	Net::takeSignal(14, [&](){ udpConfirmed=true; });
	Net::takeSignal(17, &UI::iGotKicked, this);
	Net::takeSignal(18, [&](){ quickMsg("You need to reach rank 0 to join the Hero room"); });
	Net::takeSignal(19, [&](){ onlineplayUI->matchButton->setText("Leave 1vs1 matchmaking"); });
	Net::takeSignal(20, [&](){ onlineplayUI->matchButton->setText("Join 1vs1 matchmaking"); });
	Net::takeSignal(21, [&](){
		quickMsg("You were removed from the matchmaking queue");
		onlineplayUI->matchButton->setText("Join 1vs1 matchmaking");
	});
	Net::takeSignal(22, [&](){ quickMsg("You are still in the matchmaking queue"); });

	if (options.theme == 1)
		lightTheme();
	else
		darkTheme();

	if (!options.animatedBackground)
		animatedBackground->disable();

	setOnChatFocus(resources.gfx->tGui.getWidgets());
}

UI::~UI() {
	delete bugReport;
	delete areYouSure;
	delete performanceOutput;
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

void UI::joinRoom(int id) {
	Signals::SendSig(0, id);
	away=false;
	game.autoaway=false;
}

void UI::leaveRoom() {
	Signals::SendSig(1);
	setGameState(GameStates::MainMenu);
}

void UI::setGameState(GameStates state) {
	if (gamestate == GameStates::MainMenu) { // Reset depending on what state we come from
		mainMenu->hide();
		onlineplayUI->hide();
		if (state != GameStates::MainMenu) {
			gameFieldDrawer.show();
			gameStandings->hide();
		}
	}
	else if (gamestate == GameStates::Practice) {
		game.sendgameover = false;
		game.winner = false;
		game.autoaway = false;
		if (state == GameStates::GameOver) {
			game.field.text.away=false;
			game.field.text.ready=true;
		}
	}
	else if (gamestate == GameStates::Spectating) {
		if (state != GameStates::Spectating)
			Signals::SendSig(20);
	}
	else if (gamestate == GameStates::Replay) {
		replayUI->playPause->setText("Play");
	}

	if (state == GameStates::MainMenu) { // Set depending on what state we are going into
		gameFieldDrawer.removeAllFields();
		game.field.clear();
		away=false;
		game.autoaway=false;
		game.field.text.away=false;
		game.sendgameover=false;
		game.winner=false;
		scoreScreen->hide();
		chatScreen->sendTo("Lobby");
		if (resources.playonline) {
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
	else if (state == GameStates::CountDown) {
        game.sRKey();
        game.sLKey();
        game.sDKey();
        game.gameover=false;
        game.showPressEnterText=false;
        game.draw();
	}
	else if (state == GameStates::Game || state == GameStates::Practice) {
		linesSent=0;
        garbageCleared=0;
        linesBlocked=0;
        gamedatacount=0;
		gamedata=sf::seconds(0);
		if (state == GameStates::Practice) {
			if (!game.field.text.ready)
				Signals::Ready();
			if (away)
				Signals::SetAway(false);
			game.field.clear();
		}
		game.showPressEnterText=false;
        game.startGame();
	}
	else if (state == GameStates::GameOver) {
        if (game.autoaway)
        	Signals::SetAway(true);
        if (game.sendgameover)
            sendGameOver();
        if (game.winner)
            sendGameWinner();
        if (gamestate != GameStates::Replay)
        	game.showPressEnterText=true;
        else
        	replayUI->pauseTime=sf::seconds(0);
        game.field.text.setCountdown(0);
        game.draw();
	}
	else if (state == GameStates::Replay) {
		game.showPressEnterText=false;
		game.startReplay();
	}

	gamestate = state;
}

void UI::disconnect() {
	resources.playonline=false;
	udpConfirmed=false;
	performanceOutput->ping->hide();
	onlineplayUI->matchButton->setText("Join 1vs1 matchmaking");
	serverUI->clear();
	chatScreen->hide();
	onlineplayUI->hide();
	areYouSure->hide();
	loginBox->connectingScreen.hide();
	challengesGameUI->hide();
	replayUI->hide();
	setGameState(GameStates::MainMenu);
	quickMsg("Disconnected from server");
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

void UI::receiveRecording(sf::Packet &packet) {
	sf::Uint16 type;
	packet >> type;
	game.recorder.receiveRecording(packet);
	setGameState(GameStates::Replay);
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

	if (loginBox->connectingScreen.isVisible())
		loginBox->checkStatus();

	if (resources.playonline) {
		if (!udpConfirmed)
			if (currentTime - udpPortTime > sf::milliseconds(500)) {
				udpPortTime = currentTime;
				sf::Packet packet;
				packet << (sf::Uint8)99 << myId;
				Signals::SendPacketUDP(packet);
			}
		if (onlineplayUI->isVisible()) {
			if (onlineplayUI->roomList.isVisible())
				if (currentTime - onlineplayUI->updateRoomListTime > sf::seconds(5)) {
					onlineplayUI->updateRoomListTime = currentTime;
					Signals::SendSig(16);
				}
			if (onlineplayUI->tournamentList.isVisible())
				if (currentTime - onlineplayUI->updateTournamentListTime > sf::seconds(5)) {
					onlineplayUI->updateTournamentListTime = currentTime;
					Signals::SendSig(15);
				}
		}
		if (challengesGameUI->isVisible() && (gamestate == GameStates::Game || gamestate == GameStates::Replay))
			challengesGameUI->update();
		performanceOutput->setPing(ping.send(currentTime, myId));

		if (gamestate == GameStates::CountDown) {
			sf::Uint8 cd = countdown.check(currentTime);
			if (cd != 255) {
				if (game.countDown(cd))
					setGameState(GameStates::Game);
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

	resources.gfx->tGui.handleEvent(event);
}

void UI::gameInput(sf::Event& event) {
	if (gamestate != GameStates::Replay && gamestate != GameStates::MainMenu) {
		if (event.type == sf::Event::KeyPressed) {
            if (event.key.code == options.score)
                scoreScreen->show();
            else if (event.key.code == options.away && resources.playonline && gamestate != GameStates::Spectating)
                Signals::Away();
		}
		else if (event.type == sf::Event::KeyReleased)
			if (event.key.code == options.score)
				scoreScreen->hide();
	}
	if (gamestate == GameStates::CountDown) {
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
	else if (gamestate == GameStates::Game || gamestate == GameStates::Practice) {
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
	else if (gamestate == GameStates::GameOver) {
		if (event.type == sf::Event::KeyPressed && !chatFocused) {
            if (event.key.code == sf::Keyboard::P && !areYouSure->isVisible()) {
            	game.gameover=false;
            	if (resources.playonline) {
            		if (challengesGameUI->isVisible())
            			Signals::Ready();
            		else
            			setGameState(GameStates::Practice);
            	}
            	else {
	                setGameState(GameStates::CountDown);
	                game.startCountdown();
	                game.gameover=false;
            	}
            }
            else if (event.key.code == options.ready && resources.playonline)
            	Signals::Ready();
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
    resources.gfx->tGui.setView(view);
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
			else if (gamestate != GameStates::MainMenu) {
				areYouSure->label->setText("Leave this game?");
				areYouSure->show();
			}
		}
		else if (event.key.code == sf::Keyboard::Return) {
			if (event.key.alt)
				toggleFullscreen();
			else if (areYouSure->isVisible())
				areYouSure->ausY();
			else if (resources.playonline && !chatScreen->isActive()) {
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
	resources.gfx->themeTG->setProperty("Panel", "BackgroundColor", sf::Color(25,25,25,200));
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
	resources.gfx->themeTG->setProperty("Panel", "BackgroundColor", sf::Color(230,230,230,200));
	options.theme=1;
	for (auto& field : gameFieldDrawer.fields)
		field.text.setColor(sf::Color(0,0,0));
}

void UI::setWidgetTextColor(sf::Color color) {
	resources.gfx->themeTG->setProperty("Label", "TextColor", color);
	
	color.a = 215; resources.gfx->themeTG->setProperty("Button", "TextColorNormal", color);
	color.a = 235; resources.gfx->themeTG->setProperty("Button", "TextColorHover", color);
	resources.gfx->themeTG->setProperty("Button", "TextColorDown", color);

	color.a = 215; resources.gfx->themeTG->setProperty("CheckBox", "TextColorNormal", color);
	color.a = 235; resources.gfx->themeTG->setProperty("CheckBox", "TextColorHover", color);
	color.a = 180; resources.gfx->themeTG->setProperty("CheckBox", "CheckColorNormal", color);
	color.a = 200; resources.gfx->themeTG->setProperty("CheckBox", "CheckColorHover", color);

	color.a = 180; resources.gfx->themeTG->setProperty("EditBox", "TextColor", color);
	color.a = 225; resources.gfx->themeTG->setProperty("EditBox", "SelectedTextColor", color);
	color.a = 180; resources.gfx->themeTG->setProperty("EditBox", "DefaultTextColor", color);
	color.a = 215; resources.gfx->themeTG->setProperty("EditBox", "CaretColor", color);

	color.a = 245; resources.gfx->themeTG->setProperty("ListBox", "SelectedTextColor", color);
	color.a = 215; resources.gfx->themeTG->setProperty("ListBox", "TextColorNormal", color);
	color.a = 235; resources.gfx->themeTG->setProperty("ListBox", "TextColorHover", color);

	color.a = 215; resources.gfx->themeTG->setProperty("Tab", "TextColor", color);
	color.a = 245; resources.gfx->themeTG->setProperty("Tab", "SelectedTextColor", color);

	color.a = 215; resources.gfx->themeTG->setProperty("RadioButton", "TextColorNormal", color);
	color.a = 235; resources.gfx->themeTG->setProperty("RadioButton", "TextColorHover", color);
	color.a = 180; resources.gfx->themeTG->setProperty("RadioButton", "CheckColorNormal", color);
	color.a = 200; resources.gfx->themeTG->setProperty("RadioButton", "CheckColorHover", color);

	color.a = 180; resources.gfx->themeTG->setProperty("TextBox", "TextColor", color);
	color.a = 225; resources.gfx->themeTG->setProperty("TextBox", "SelectedTextColor", color);
	color.a = 215; resources.gfx->themeTG->setProperty("TextBox", "CaretColor", color);
}

void UI::setOnChatFocus(const std::vector<tgui::Widget::Ptr> widgets) {
	for (auto& widget : widgets) {
		if (widget->getWidgetType() == "Panel")
			setOnChatFocus(std::static_pointer_cast<tgui::Panel>(widget)->getWidgets());
		else if (widget->getWidgetType() == "EditBox" || widget->getWidgetType() == "TextBox")
			widget->connect("Focused Unfocused", &UI::chatFocus, this, std::bind(&tgui::Widget::isFocused, widget));
	}
}

void UI::getGameState(sf::Packet& packet) {
	sf::Uint16 clientid;
	sf::Uint8 datacount;
	packet >> clientid >> datacount;
	for (auto&& field : gameFieldDrawer.fields)
		if (field.id==clientid) {
			if (datacount>field.datacount || (datacount<50 && field.datacount>200)) {
				field.datacount=datacount;
				for (int c=0; packet >> compressor.tmp[c]; c++) {}
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
		Signals::SendSig(2, amount);
		linesSent = game.linesSent;
	}

	if (game.garbageCleared > garbageCleared) {
		sf::Uint8 amount = game.garbageCleared-garbageCleared;
		Signals::SendSig(3, amount);
		garbageCleared = game.garbageCleared;
	}

	if (game.garbage.linesBlocked > linesBlocked) {
		sf::Uint8 amount = game.garbage.linesBlocked-linesBlocked;
		Signals::SendSig(4, amount);
		linesBlocked = game.garbage.linesBlocked;
	}
}

void UI::sendGameState() {
	if (gamestate == GameStates::CountDown) {
		sf::Uint8 tmp = game.field.piece.piece;
		game.field.piece.piece = 7; // makes the current piece not draw on other players screen (since it's countdown)
		compressor.compress();
		game.field.piece.piece = tmp;
	}
	else
		compressor.compress();
	sf::Packet packet;
	packet << (sf::Uint8)100 << myId << gamedatacount;
	gamedatacount++;
	for (int i=0; i<compressor.tmpcount; i++)
		packet << compressor.tmp[i];
	if (compressor.bitcount>0)
		packet << compressor.tmp[compressor.tmpcount];
	Signals::SendPacketUDP(packet);
}

void UI::sendGameOver() {
	sf::Packet packet;
	packet << (sf::Uint8)3 << game.combo.maxCombo << game.linesSent << game.linesRecieved << game.garbage.linesBlocked << game.bpm;
	Signals::SendPacket(packet);
	game.sendgameover=false;

	sendGameState();
}

void UI::sendGameWinner() {
	sf::Packet packet;
	packet << (sf::Uint8)4 << game.combo.maxCombo << game.linesSent << game.linesRecieved << game.garbage.linesBlocked;
	packet << game.bpm << (sf::Uint32)game.recorder.duration.asMilliseconds();
	packet << (sf::Uint16)game.pieceCount;
	Signals::SendPacket(packet);
	game.winner=false;

	sendGameState();
}

void UI::iGotKicked(sf::Uint16 reason) {
	if (reason == 1)
		quickMsg("Kicked: game Clock out of sync");
	else
		quickMsg("Kicked from room for unknown reason");
	setGameState(GameStates::MainMenu);
}

void UI::setCountdown(sf::Packet &packet) {
	countdown.set(delayClock.getElapsedTime(), packet);
	if (countdown.ongoing() && !away && gamestate != GameStates::CountDown) {
		setGameState(GameStates::CountDown);
		game.startCountdown();
	}
}

void UI::showElement(int elem) {
	guiElements[elem]->show();
}

void UI::hideElement(int elem) {
	guiElements[elem]->hide();
}

void UI::enableElement(int elem) {
	guiElements[elem]->enable();
}

void UI::disableElement(int elem) {
	guiElements[elem]->disable();
}

bool UI::isVisible(int elem) {
	return guiElements[elem]->isVisible();
}

// ---------- new stuff

void UI::joinRoomResponse(sf::Packet &packet) {
	sf::Uint16 joinok;
	packet >> joinok;
	if (joinok == 1 || joinok == 1000) {
		sf::Uint8 playersinroom;
		sf::Uint16 playerid, seed1, seed2;
		packet >> seed1 >> seed2 >> playersinroom;
		game.rander.seedPiece(seed1);
		game.rander.seedHole(seed2);
		game.rander.reset();

		if (joinok == 1) {
			setGameState(GameStates::GameOver);
			game.pressEnterText.setString("press P to start practice");
			game.field.clear();
			game.draw();
			gameFieldDrawer.setPosition(465, 40);
			gameFieldDrawer.setSize(490, 555);
		}
		else {
			setGameState(GameStates::Spectating);
			gameFieldDrawer.setPosition(5, 40);
			gameFieldDrawer.setSize(950, 555);
		}

		obsField newfield(resources);
		newfield.clear();
		sf::String name;
		for (int c=0; c<playersinroom; c++) {
			packet >> playerid >> name;
			newfield.id = playerid;
			gameFieldDrawer.addField(newfield);
			gameFieldDrawer.fields.back().text.setName(name);
			gameFieldDrawer.fields.back().drawField();
		}
		if (gamestate == GameStates::Spectating)
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
		setGameState(GameStates::GameOver);
		gameFieldDrawer.hide();
		game.pressEnterText.setString("press P to start challenge");
		game.field.clear();
		game.draw();
		challengesGameUI->show();
		challengesGameUI->showPanel(joinok);
		challengesGameUI->clear();
	}
}

void UI::getAuthResult(sf::Packet &packet) {
	if (loginBox->t.joinable())
		loginBox->t.join();
	sf::Uint8 success;
	packet >> success;
	if (success == 1) {
		sf::String name;
		packet >> resources.name >> myId;
		game.field.text.setName(resources.name);
		loginBox->connectingScreen.hide();
		onlineplayUI->show();
		onlineplayUI->opTab->select(0);
		serverUI->putClient(myId, name);
	}
	else if (success == 2) {
		loginBox->connectingScreen.hide();
		onlineplayUI->show();
		onlineplayUI->opTab->select(0);
		serverUI->putClient(myId, game.field.text.name);
	}
	else {
		disconnect();
		if (success == 3) {
			loginBox->connectingScreen.label->setText("You have the wrong client version, attempting to patch...");
			performanceOutput->ping->hide();
			loginBox->t = std::thread(&PatchCheck::check, &loginBox->patcher, resources.clientVersion);
			return;
		}
		else if (success == 4)
			quickMsg("Name already in use");
		else
			quickMsg("Authentication failed");
		loginBox->connectingScreen.hide();
		mainMenu->show();
		loginBox->show();
		performanceOutput->ping->hide();
	}
}