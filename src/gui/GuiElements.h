#ifndef GUIELEMENTS_H
#define GUIELEMENTS_H

#include <vector>

#include "AlertsUI.h"
#include "AnimatedBackground.h"
#include "AreYouSure.h"
#include "BugReport.h"
#include "ChallengesGameUI.h"
#include "ChatScreen.h"
#include "GameFieldDrawer.h"
#include "GameOptions.h"
#include "GameStandings.h"
#include "LoginBox.h"
#include "MainMenu.h"
#include "OnlineplayUI.h"
#include "PerformanceOutput.h"
#include "PopupMenu.h"
#include "ReplayUI.h"
#include "ScoreScreen.h"
#include "ServerUI.h"
#include "SlideMenu.h"
#include "TrainingUI.h"

class Resources;

struct GuiElements {
    GuiElements(Resources& _resources);
    void delayCheck(const sf::Time& currentTime);
    bool keyEvents(sf::Event& event);
    void windowEvents(sf::Event& event);
    void mouseEvents(sf::Event& event);
    void resizeWindow(sf::Event& event);
    bool handleEvent(sf::Event& event);
    void toggleFullscreen();

    void getAuthResult(sf::Packet& packet);

    Resources& resources;

    AnimatedBackground animatedBackground;
    Menu mainMenu;
    LoginBox loginBox;
    ChallengesGameUI challengesGameUI;
    OnlineplayUI onlineplayUI;
    GameStandings gameStandings;
    ReplayUI replayUI;
    TrainingUI trainingUI;
    PerformanceOutput performanceOutput;
    ChatScreen chatScreen;
    SlideMenu slideMenu;
    GameOptions gameOptions;
    BugReport bugReport;
    ServerUI serverUI;
    AlertsUI alertsUI;
    AreYouSure areYouSure;
    os::Label QuickMsg;
    ScoreScreen scoreScreen;

    GameFieldDrawer gameFieldDrawer;

    PopupMenu player_popup;

    bool udpConfirmed;

    std::vector<GuiBase*> elements;
    sf::Time quickMsgTime;
};

#endif
