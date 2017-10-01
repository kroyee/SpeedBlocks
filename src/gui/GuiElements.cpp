#include "GuiElements.h"
#include "Resources.h"
#include "textures.h"
#include "optionSet.h"
#include "Signal.h"
#include "gameField.h"
#include <SFML/Network.hpp>

GuiElements::GuiElements(Resources &_resources) :
resources(_resources),
animatedBackground		(resources, 7),
mainMenu				(sf::Rect<int>(0,0,960,600), resources),
loginBox				(sf::Rect<int>(500,50,450,550), resources, mainMenu.panel),
challengesGameUI		(sf::Rect<int>(0,0,960,600), resources),
onlineplayUI			(sf::Rect<int>(0,0,960,600), resources),
gameStandings			(sf::Rect<int>(330,185,120,100), resources),
replayUI				(sf::Rect<int>(465,530,490,70), resources),
performanceOutput		(sf::Rect<int>(807,0,113,28), resources),
chatScreen				(sf::Rect<int>(440,0,480,600), resources),
slideMenu				(sf::Rect<int>(920,0,600,600), resources),
gameOptions				(sf::Rect<int>(40,40,560,560), resources, slideMenu.panel),
bugReport				(sf::Rect<int>(40,40,560,560), resources, slideMenu.panel),
serverUI				(sf::Rect<int>(40,40,560,560), resources, slideMenu.panel),
alertsUI				(sf::Rect<int>(40,40,560,560), resources, slideMenu.panel),
areYouSure				(sf::Rect<int>(0,0,960,600), resources),
scoreScreen				(sf::Rect<int>(30,30,860,540), resources),
gameFieldDrawer			(resources),
udpConfirmed			(false)
{
	mainMenu.show();
	loginBox.show();

	QuickMsg = tgui::Label::create();
	QuickMsg->setPosition(0,10);
	QuickMsg->setSize(960,90);
	QuickMsg->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Center);
	QuickMsg->setTextSize(32);
	QuickMsg->setTextColor(sf::Color::Red);
	QuickMsg->hide();
	QuickMsg->disable(false);
	resources.gfx->tGui.add(QuickMsg);

	slideMenu.show();
	alertsUI.show();

	if (resources.options->performanceOutput)
		performanceOutput.show();

	if (!resources.options->animatedBackground)
		animatedBackground.disable();

	elements.push_back(&mainMenu);
	elements.push_back(&loginBox);
	elements.push_back(&gameOptions);
	elements.push_back(&loginBox.connectingScreen);
	elements.push_back(&onlineplayUI);
	elements.push_back(&areYouSure);
	elements.push_back(&performanceOutput);
	elements.push_back(&bugReport);
	elements.push_back(&challengesGameUI);
	elements.push_back(&replayUI);
	elements.push_back(&gameStandings);
	elements.push_back(&chatScreen);
	elements.push_back(&slideMenu);
	elements.push_back(&scoreScreen);
	elements.push_back(&serverUI);
	elements.push_back(&alertsUI);

	Signals::Show.connect([&](int elem){ elements[elem]->show(); });
	Signals::Hide.connect([&](int elem){ elements[elem]->hide(); });
	Signals::Enable.connect([&](int elem){ elements[elem]->enable(); });
	Signals::Disable.connect([&](int elem){ elements[elem]->disable(); });
	Signals::IsVisible.connect([&](int elem){ return elements[elem]->isVisible(); });
	Signals::QuickMsg.connect([&](const sf::String& msg){
		QuickMsg->setText(msg);
		QuickMsg->show();
		quickMsgTime = resources.delayClock.getElapsedTime();
	});
	Signals::Disconnect.connect([&](int showMsg){
		resources.playonline=false;
		udpConfirmed=false;
		performanceOutput.ping->hide();
		onlineplayUI.matchButton->setText("Join 1vs1 matchmaking");
		serverUI.clear();
		chatScreen.hide();
		onlineplayUI.hide();
		areYouSure.hide();
		loginBox.connectingScreen.hide();
		challengesGameUI.hide();
		replayUI.hide();
		Signals::SetGameState(GameStates::MainMenu);
		if (showMsg)
			Signals::QuickMsg("Disconnected from server");
	});

	Net::takePacket(0, [&](sf::Packet& packet){
		sf::String welcomeMsg;
		packet >> resources.myId >> welcomeMsg;
		serverUI.motd->setText(welcomeMsg);
		onlineplayUI.makeRoomList(packet);
		serverUI.makeClientList(packet);
		performanceOutput.ping->show();
	});
	Net::takePacket(4, [&](sf::Packet &packet){
		sf::String name;
		obsField newfield(resources);
		newfield.clear();
		packet >> newfield.id >> name;
		gameFieldDrawer.addField(newfield);
		gameFieldDrawer.fields.back().text.setName(name);
		gameFieldDrawer.fields.back().drawField();
		if (gameStandings.isVisible())
			gameStandings.alignResult();
	});
	Net::takePacket(7, [&](sf::Packet &packet){
		sf::String text;
		packet >> text;
		Signals::QuickMsg("Your score of " + text);
	});

	Net::takeSignal(6, [&](sf::Uint16 id1){
		gameFieldDrawer.removeField(id1);
		if (gameStandings.isVisible())
			gameStandings.alignResult();
	});
	Net::takePacket(9, &GuiElements::getAuthResult, this);
	Net::takeSignal(14, [&](){ udpConfirmed=true; });
	Net::takeSignal(19, [&](){ onlineplayUI.matchButton->setText("Leave 1vs1 matchmaking"); });
	Net::takeSignal(20, [&](){ onlineplayUI.matchButton->setText("Join 1vs1 matchmaking"); });
	Net::takeSignal(21, [&](){
		Signals::QuickMsg("You were removed from the matchmaking queue");
		onlineplayUI.matchButton->setText("Join 1vs1 matchmaking");
	});
	Net::takeSignal(17, [&](sf::Uint16 reason){
		if (reason == 1)
			Signals::QuickMsg("Kicked: game Clock out of sync");
		else
			Signals::QuickMsg("Kicked from room for unknown reason");
		Signals::SetGameState(GameStates::MainMenu);
	});
}

void GuiElements::getAuthResult(sf::Packet &packet) {
	if (loginBox.t.joinable())
		loginBox.t.join();
	sf::Uint8 success;
	packet >> success;
	if (success == 1) {
		packet >> resources.name >> resources.myId;
		Signals::SetName(resources.name);
		loginBox.connectingScreen.hide();
		onlineplayUI.show();
		onlineplayUI.opTab->select(0);
		serverUI.putClient(resources.myId, resources.name);
	}
	else if (success == 2) {
		loginBox.connectingScreen.hide();
		onlineplayUI.show();
		onlineplayUI.opTab->select(0);
		serverUI.putClient(resources.myId, resources.name);
	}
	else {
		Signals::Disconnect(0);
		if (success == 3) {
			loginBox.connectingScreen.label->setText("You have the wrong client version, attempting to patch...");
			performanceOutput.ping->hide();
			loginBox.t = std::thread(&PatchCheck::check, &loginBox.patcher, resources.clientVersion);
			return;
		}
		else if (success == 4)
			Signals::QuickMsg("Name already in use");
		else
			Signals::QuickMsg("Authentication failed");
		loginBox.connectingScreen.hide();
		mainMenu.show();
		loginBox.show();
		performanceOutput.ping->hide();
	}
}

void GuiElements::delayCheck(const sf::Time& currentTime) {
	if (QuickMsg->isVisible())
		if (currentTime - quickMsgTime > sf::seconds(5))
			QuickMsg->hide();

	if (loginBox.connectingScreen.isVisible())
		loginBox.checkStatus();

	if (resources.playonline) {
		if (onlineplayUI.isVisible()) {
			if (onlineplayUI.roomList.isVisible())
				if (currentTime - onlineplayUI.updateRoomListTime > sf::seconds(5)) {
					onlineplayUI.updateRoomListTime = currentTime;
					Signals::SendSig(16);
				}
			if (onlineplayUI.tournamentList.isVisible())
				if (currentTime - onlineplayUI.updateTournamentListTime > sf::seconds(5)) {
					onlineplayUI.updateTournamentListTime = currentTime;
					Signals::SendSig(15);
				}
		}
	}

	slideMenu.update(currentTime);

	if (bugReport.join)
		if (bugReport.t.joinable())
			bugReport.t.join();

	chatScreen.fade(currentTime);
}

void GuiElements::toggleFullscreen() {
	if (!resources.options->fullscreen) {
		if (resources.options->currentmode == -1)
			resources.options->currentmode = 0;
		resources.window.close();
		resources.window.create(resources.options->modes[resources.options->currentmode], "SpeedBlocks", sf::Style::Fullscreen);
		resources.window.setView(sf::View(sf::FloatRect(0, 0, 960, 600)));
		resources.options->fullscreen=true;
	}
	else if (resources.options->fullscreen || !resources.window.isOpen()) {
		resources.window.close();
		resources.window.create(sf::VideoMode(960, 600), "SpeedBlocks");
		resources.window.setView(sf::View(sf::FloatRect(0, 0, 960, 600)));
		resources.options->fullscreen=false;
	}
}

bool GuiElements::keyEvents(sf::Event& event) {
	if (event.type == sf::Event::KeyPressed) {
		if (chatScreen.isActive())
			chatScreen.focus();
		if (event.key.code == sf::Keyboard::Escape) {
			if (areYouSure.isVisible())
				areYouSure.hide();
			else if (chatScreen.isActive())
				chatScreen.deactivate();
			else if (mainMenu.isVisible()) {
				areYouSure.label->setText("Do you want to quit?");
				areYouSure.show();
			}
			else if (onlineplayUI.isVisible()) {
				areYouSure.label->setText("Leave the server?");
				areYouSure.show();
			}
			else if (challengesGameUI.isVisible()) {
				areYouSure.label->setText("Leave this challenge?");
				areYouSure.show();
			}
			else if (resources.gamestate != GameStates::MainMenu) {
				areYouSure.label->setText("Leave this game?");
				areYouSure.show();
			}
		}
		else if (event.key.code == sf::Keyboard::Return) {
			if (event.key.alt)
				toggleFullscreen();
			else if (areYouSure.isVisible())
				areYouSure.ausY();
			else if (resources.playonline && !chatScreen.isActive()) {
				chatScreen.activate();
				return true;
			}
		}
		else if (event.key.code == sf::Keyboard::Tab && chatScreen.isActive()) {
			chatScreen.toggleSendTo();
			return true;
		}
	}
	return false;
}

void GuiElements::windowEvents(sf::Event& event) {
	if (event.type == sf::Event::Closed)
        resources.window.close();
    else if (event.type == sf::Event::Resized && !resources.options->fullscreen)
        resizeWindow(event);
    else if (event.type == sf::Event::LostFocus)
    	resources.window.setFramerateLimit(60);
    else if (event.type == sf::Event::GainedFocus)
    	resources.window.setFramerateLimit(0);
}

void GuiElements::resizeWindow(sf::Event& event) {
	sf::View view = resources.window.getView();
    float ratio;
    if ((float)event.size.width/event.size.height > 960.0/600) {
        ratio = (event.size.height * (960.0/600)) / event.size.width;
        view.setViewport(sf::FloatRect((1-ratio)/2.0, 0, ratio, 1));
    }
    else {
        ratio = (event.size.width / (960.0/600)) / event.size.height;
        view.setViewport(sf::FloatRect(0, (1-ratio)/2.0, 1, ratio));
    }
    resources.window.setView(view);
    resources.gfx->tGui.setView(view);
    resources.window.clear(sf::Color(0,0,0));
}

bool GuiElements::handleEvent(sf::Event& event) {
	windowEvents(event);

	if (gameOptions.SelectKey->isVisible())
		if (gameOptions.putKey(event))
			return true;

	if (keyEvents(event))
		return true;

	if (gameFieldDrawer.isVisible())
		gameFieldDrawer.enlargePlayfield(event);

	if (onlineplayUI.isVisible()) {
		onlineplayUI.roomList.scrolled(event);
		onlineplayUI.tournamentList.scrolled(event);
		onlineplayUI.challengesUI.challengeList.scrolled(event);
		onlineplayUI.challengesUI.scrolled(event);
	}

	slideMenu.handleEvent(event);
	return false;
}