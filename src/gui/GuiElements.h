#ifndef GUIELEMENTS_H
#define GUIELEMENTS_H

#include <vector>

#include "MainMenu.h"
#include "LoginBox.h"
#include "GameOptions.h"
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
#include "GameFieldDrawer.h"

class Resources;

struct GuiElements {
	GuiElements(Resources &_resources);
	void delayCheck(const sf::Time& currentTime);
	bool keyEvents(sf::Event& event);
	void windowEvents(sf::Event& event);
	void resizeWindow(sf::Event& event);
	bool handleEvent(sf::Event& event);
	void toggleFullscreen();

	void getAuthResult(sf::Packet &packet);

	Resources &resources;


	AnimatedBackground animatedBackground;
	Menu mainMenu;
	LoginBox loginBox;
	ChallengesGameUI challengesGameUI;
	OnlineplayUI onlineplayUI;
	GameStandings gameStandings;
	ReplayUI replayUI;
	PerformanceOutput performanceOutput;
	ChatScreen chatScreen;
	SlideMenu slideMenu;
	GameOptions gameOptions;
	BugReport bugReport;
	ServerUI serverUI;
	AlertsUI alertsUI;
	AreYouSure areYouSure;
	tgui::Label::Ptr QuickMsg;
	ScoreScreen scoreScreen;

	GameFieldDrawer gameFieldDrawer;

	bool udpConfirmed;

	std::vector<guiBase*> elements;
	sf::Time quickMsgTime;
};

#endif