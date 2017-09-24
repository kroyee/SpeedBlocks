#include "gui.h"
#include "optionSet.h"
#include "gamePlay.h"
#include "textures.h"
#include "Signal.h"
#include "GuiElements.h"
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
      guiElements(new GuiElements(resources)),
      window(&window_),
      away(false),
      linesSent(0),
      garbageCleared(0),
      linesBlocked(0),
      delayClock(resources.delayClock),
      gamestate(game_.resources.gamestate),
      gamedata(sf::seconds(0)),
      gamedatacount(0),
      myId(resources.myId) {

    compressor.game=&game;

	Signals::LeaveRoom.connect(&UI::leaveRoom, this);
	Signals::SetGameState.connect(&UI::setGameState, this);
	Signals::LightTheme.connect(&UI::lightTheme, this);
	Signals::DarkTheme.connect(&UI::darkTheme, this);
	Signals::JoinRoom.connect(&UI::joinRoom, this);

	Net::takePacket(100, &UI::getGameState, this);
	Net::takePacket(1, &UI::receiveRecording, this);
	Net::takePacket(3, &UI::joinRoomResponse, this);
	Net::takePacket(6, [&](sf::Packet &packet){
		sf::String text;
		packet >> text;
		Signals::QuickMsg("You improved your score from " + text);
		game.recorder.sendRecording(guiElements->onlineplayUI.challengesUI.selectedId);
	});
	Net::takePacket(102, [&](sf::Packet &packet){
		guiElements->performanceOutput.setPing(ping.get(delayClock.getElapsedTime(), packet));
	});
	Net::takePacket(103, &UI::setCountdown, this);

	Net::takeSignal(0, [&](){ Signals::QuickMsg("Not enough players to start tournament"); });
	Net::takeSignal(2, [&](){ Signals::QuickMsg("You can't do that as guest, register at https://speedblocks.se"); });
	Net::takeSignal(4, [&](sf::Uint16 id1, sf::Uint16 id2){
		game.rander.seedPiece(id1);
		game.rander.seedHole(id2);
		game.rander.reset();
		game.startCountdown();
		game.countDown(3);
		guiElements->gameFieldDrawer.resetOppFields();
		setGameState(GameStates::CountDown);
		gamedatacount=251;
		sendGameState();
		countdown.start(delayClock.getElapsedTime());
		if (guiElements->challengesGameUI.isVisible()) {
			guiElements->challengesGameUI.clear();
			if (guiElements->challengesGameUI.cheesePanel->isVisible()) {
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
		guiElements->gameFieldDrawer.resetOppFields();
		game.rander.seedPiece(id1);
		game.rander.seedHole(id2);
		game.rander.reset();
		if (gamestate != GameStates::Spectating) {
			game.field.clear();
			game.draw();
		}
		countdown.start(delayClock.getElapsedTime());
	});
	Net::takeSignal(18, [&](){ Signals::QuickMsg("You need to reach rank 0 to join the Hero room"); });
	Net::takeSignal(22, [&](){ Signals::QuickMsg("You are still in the matchmaking queue"); });

	if (options.theme == 1)
		lightTheme();
	else
		darkTheme();

	setOnChatFocus(resources.gfx->tGui.getWidgets());
}

UI::~UI() {
	delete guiElements;
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
		guiElements->mainMenu.hide();
		guiElements->onlineplayUI.hide();
		if (state != GameStates::MainMenu) {
			guiElements->gameFieldDrawer.show();
			guiElements->gameStandings.hide();
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
		guiElements->replayUI.playPause->setText("Play");
	}

	if (state == GameStates::MainMenu) { // Set depending on what state we are going into
		guiElements->gameFieldDrawer.removeAllFields();
		game.field.clear();
		away=false;
		game.autoaway=false;
		game.field.text.away=false;
		game.sendgameover=false;
		game.winner=false;
		guiElements->scoreScreen.hide();
		guiElements->chatScreen.sendTo("Lobby");
		if (resources.playonline) {
			guiElements->onlineplayUI.show();
			guiElements->gameFieldDrawer.hide();
			guiElements->challengesGameUI.hide();
			guiElements->replayUI.hide();
			guiElements->gameStandings.hide();
		}
		else {
			guiElements->gameFieldDrawer.hide();
			guiElements->mainMenu.show();
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
        	guiElements->replayUI.pauseTime=sf::seconds(0);
        game.field.text.setCountdown(0);
        game.draw();
	}
	else if (state == GameStates::Replay) {
		game.showPressEnterText=false;
		game.startReplay();
	}

	gamestate = state;
}

void UI::chatFocus(bool i) {
	if (i) {
		resources.chatFocused=true;
		window->setKeyRepeatEnabled(true);
	}
	else {
		resources.chatFocused=false;
		window->setKeyRepeatEnabled(false);
	}
}

void UI::receiveRecording(sf::Packet &packet) {
	sf::Uint16 type;
	packet >> type;
	game.recorder.receiveRecording(packet);
	setGameState(GameStates::Replay);
	if (type >= 20000) {
		guiElements->gameFieldDrawer.hide();
		guiElements->challengesGameUI.show();
		guiElements->challengesGameUI.showPanel(type);
		guiElements->replayUI.show();
	}
}

void UI::delayCheck() {
	sf::Time currentTime = delayClock.getElapsedTime();
	guiElements->delayCheck(currentTime);

	if (resources.playonline) {
		if (!guiElements->udpConfirmed)
			if (currentTime - udpPortTime > sf::milliseconds(500)) {
				udpPortTime = currentTime;
				sf::Packet packet;
				packet << (sf::Uint8)99 << myId;
				Signals::SendPacketUDP(packet);
			}

		if (gamestate == GameStates::CountDown) {
			sf::Uint8 cd = countdown.check(currentTime);
			if (cd != 255) {
				if (game.countDown(cd))
					setGameState(GameStates::Game);
				gamedatacount=255-cd;
				sendGameState();
			}
		}

		guiElements->performanceOutput.setPing(ping.send(currentTime, myId));
	}
}

bool UI::handleEvent(sf::Event& event) {
	if (guiElements->handleEvent(event))
		return false;

	resources.gfx->tGui.handleEvent(event);
	return true;
}

void UI::darkTheme() {
	guiElements->animatedBackground.dark();
	guiElements->slideMenu.dark();
	guiElements->chatScreen.dark();
	game.field.text.setColor(sf::Color(255,255,255));
	game.pressEnterText.setFillColor(sf::Color(255,255,255));
	game.pressEnterText.setOutlineColor(sf::Color(255,255,255));
	game.draw();
	setWidgetTextColor(sf::Color(255,255,255,200));
	resources.gfx->themeTG->setProperty("Panel", "BackgroundColor", sf::Color(25,25,25,200));
	options.theme=2;
	for (auto& field : guiElements->gameFieldDrawer.fields)
		field.text.setColor(sf::Color(255,255,255));
}

void UI::lightTheme() {
	guiElements->animatedBackground.light();
	guiElements->slideMenu.light();
	guiElements->chatScreen.light();
	game.field.text.setColor(sf::Color(0,0,0));
	game.pressEnterText.setFillColor(sf::Color(0,0,0));
	game.pressEnterText.setOutlineColor(sf::Color(0,0,0));
	game.draw();
	setWidgetTextColor(sf::Color(0,0,0,200));
	resources.gfx->themeTG->setProperty("Panel", "BackgroundColor", sf::Color(230,230,230,200));
	options.theme=1;
	for (auto& field : guiElements->gameFieldDrawer.fields)
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
	for (auto&& field : guiElements->gameFieldDrawer.fields)
		if (field.id==clientid) {
			if (datacount>field.datacount || (datacount<50 && field.datacount>200)) {
				field.datacount=datacount;
				for (int c=0; packet >> compressor.tmp[c]; c++) {}
				compressor.extract();
				if (compressor.validate()) {
					compressor.field = &field;
					compressor.copy();
					guiElements->gameFieldDrawer.drawOppField(field);
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

void UI::setCountdown(sf::Packet &packet) {
	countdown.set(delayClock.getElapsedTime(), packet);
	if (countdown.ongoing() && !away && gamestate != GameStates::CountDown) {
		setGameState(GameStates::CountDown);
		game.startCountdown();
	}
}

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
			guiElements->gameFieldDrawer.setPosition(465, 40);
			guiElements->gameFieldDrawer.setSize(490, 555);
		}
		else {
			setGameState(GameStates::Spectating);
			guiElements->gameFieldDrawer.setPosition(5, 40);
			guiElements->gameFieldDrawer.setSize(950, 555);
		}

		obsField newfield(resources);
		newfield.clear();
		sf::String name;
		for (int c=0; c<playersinroom; c++) {
			packet >> playerid >> name;
			newfield.id = playerid;
			guiElements->gameFieldDrawer.addField(newfield);
			guiElements->gameFieldDrawer.fields.back().text.setName(name);
			guiElements->gameFieldDrawer.fields.back().drawField();
		}
		if (gamestate == GameStates::Spectating)
			guiElements->gameStandings.alignResult();

		guiElements->chatScreen.sendTo("Room");
	}
	else if (joinok == 2)
		Signals::QuickMsg("Room is full");
	else if (joinok == 3)
		Signals::QuickMsg("Please wait for server to get your user-data");
	else if (joinok == 4)
		Signals::QuickMsg("This is not your game");
	else if (joinok >= 20000) {
		setGameState(GameStates::GameOver);
		guiElements->gameFieldDrawer.hide();
		game.pressEnterText.setString("press P to start challenge");
		game.field.clear();
		game.draw();
		guiElements->challengesGameUI.show();
		guiElements->challengesGameUI.showPanel(joinok);
		guiElements->challengesGameUI.clear();
	}
}