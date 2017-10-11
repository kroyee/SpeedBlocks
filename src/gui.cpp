#include "gui.h"
#include "optionSet.h"
#include "gamePlay.h"
#include "textures.h"
#include "GameSignals.h"
#include "GuiElements.h"
#include "UIGameState.h"
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
      guiElements(new GuiElements(resources)),
      window(&window_),
      away(false),
      linesSent(0),
      garbageCleared(0),
      linesBlocked(0),
      delayClock(resources.delayClock),
      gamestate(game_.resources.gamestate),
      state(std::unique_ptr<UIBaseState>(new UIMainMenu(*this))),
      myId(resources.myId) {

	Signals::LeaveRoom.connect(&UI::leaveRoom, this);
	Signals::SetGameState.connect(&UI::setGameState, this);
	Signals::LightTheme.connect(&UI::lightTheme, this);
	Signals::DarkTheme.connect(&UI::darkTheme, this);
	Signals::JoinRoom.connect(&UI::joinRoom, this);

	Net::takePacket(100, &UI::getGameState, this);
	Net::takePacket(1, &UI::receiveRecording, this);
	Net::takePacket(3, &UI::joinRoomResponse, this);
	Net::takePacket(102, [&](sf::Packet &packet){
		guiElements->performanceOutput.setPing(ping.get(delayClock.getElapsedTime(), packet));
	});
	Net::takePacket(103, &UI::setCountdown, this);

	Net::takeSignal(0, [&](){ Signals::QuickMsg("Not enough players to start tournament"); });
	Net::takeSignal(2, [&](){ Signals::QuickMsg("You can't do that as guest, register at https://speedblocks.se"); });
	Net::takeSignal(4, [&](sf::Uint16 id1, sf::Uint16 id2){
		Signals::SeedRander(id1, id2);
		Signals::StartCountDown();
		guiElements->gameFieldDrawer.resetOppFields();
		setGameState(GameStates::CountDown);
		countdown.start(delayClock.getElapsedTime());
		if (guiElements->challengesGameUI.isVisible()) {
			guiElements->challengesGameUI.clear();
			if (guiElements->challengesGameUI.challenge->type == Challenges::Cheese)
				Signals::GameSetup(1);
			else if (guiElements->challengesGameUI.challenge->type == Challenges::Cheese30L)
				Signals::GameSetup(2);
		}
	});
	Net::takeSignal(5, [&](sf::Uint16 id1){
		if (!id1 && gamestate==GameStates::Game)
			Signals::GameOver(0);
		countdown.stop();
	});
	Net::takeSignal(7, [&](){
		if (gamestate != GameStates::Practice)
			Signals::GameOver(0);
		countdown.stop();
	});
	Net::takeSignal(8, [&](){
		Signals::GameOver(1);
		countdown.stop();
	});
	Net::takeSignal(10, [&](sf::Uint16 id1, sf::Uint16 id2){
		guiElements->gameFieldDrawer.resetOppFields();
		Signals::SeedRander(id1, id2);
		if (gamestate != GameStates::Spectating) {
			Signals::GameClear();
			Signals::GameDraw();
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

void UI::setGameState(GameStates _state) {
	state.reset(nullptr);
	if (_state == GameStates::MainMenu)
		state = std::unique_ptr<UIBaseState>(new UIMainMenu(*this));
	else if (_state == GameStates::CountDown)
		state = std::unique_ptr<UIBaseState>(new UICountDown(*this));
	else if (_state == GameStates::Game)
		state = std::unique_ptr<UIBaseState>(new UIGame(*this));
	else if (_state == GameStates::GameOver)
		state = std::unique_ptr<UIBaseState>(new UIGameOver(*this));
	else if (_state == GameStates::Replay)
		state = std::unique_ptr<UIBaseState>(new UIReplay(*this));
	else if (_state == GameStates::Practice)
		state = std::unique_ptr<UIBaseState>(new UIPractice(*this));
	else if (_state == GameStates::Spectating)
		state = std::unique_ptr<UIBaseState>(new UISpectating(*this));
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
		guiElements->challengesGameUI.openChallenge(type);
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

		if (gamestate == GameStates::CountDown)
			if (game.countDown(countdown.check(currentTime)))
				setGameState(GameStates::Game);

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
				for (int c=0; packet >> resources.compressor->tmp[c]; c++) {}
				resources.compressor->extract();
				if (resources.compressor->validate()) {
					resources.compressor->field = &field;
					resources.compressor->copy();
					field.drawField();
				}
			}
			break;
		}
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
		Signals::SeedRander(seed1, seed2);

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
		guiElements->challengesGameUI.openChallenge(joinok);
	}
}