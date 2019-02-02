#include "GuiElements.h"
#include <SFML/Network.hpp>
#include <iostream>
#include "GameField.h"
#include "GameSignals.h"
#include "NetworkPackets.hpp"
#include "Options.h"
#include "Resources.h"
#include "TaskQueue.h"
#include "Textures.h"

static auto& AddField = Signal<ObsField&, int, const std::string&>::get("AddField");
static auto& QuickMSG = Signal<void, const std::string&>::get("QuickMsg");
static auto& SetGameState = Signal<void, GameStates>::get("SetGameState");
static auto& SetName = Signal<void, const std::string&>::get("SetName");
static auto& Disconnect = Signal<void, int>::get("Disconnect");

static auto& Away = Signal<void>::get("Away");
static auto& Ready = Signal<void>::get("Ready");

GuiElements::GuiElements(Resources& _resources)
    : resources(_resources),
      animatedBackground(resources, 7),
      mainMenu({0, 0, 960, 600}, resources),
      loginBox({500, 50, 450, 550}, resources, mainMenu.panel),
      challengesGameUI({0, 0, 960, 600}, resources),
      onlineplayUI({0, 0, 960, 600}, resources),
      gameStandings({330, 185, 120, 100}, resources),
      replayUI({425, 530, 490, 70}, resources),
      trainingUI({50, 50, 700, 550}, resources),
      performanceOutput({807, 0, 113, 28}, resources),
      chatScreen({440, 0, 480, 600}, resources),
      slideMenu({920, 0, 600, 600}, resources),
      gameOptions({40, 40, 560, 560}, resources, slideMenu.panel),
      bugReport({40, 40, 560, 560}, resources, slideMenu.panel),
      serverUI({40, 40, 560, 560}, resources, slideMenu.panel),
      alertsUI({400, 40, 560, 560}, resources),
      areYouSure({0, 0, 960, 600}, resources),
      scoreScreen({30, 30, 860, 540}, resources),
      gameFieldDrawer(resources),
      player_popup(resources),
      udpConfirmed(false) {
    mainMenu.show();
    loginBox.show();

    resources.gfx->tGui.add(QuickMsg.get());
    QuickMsg.pos(0, 10).size(960, 90).center().text_size(32).hide()->getRenderer()->setTextColor(sf::Color::Red);

    slideMenu.show();

    if (Options::get<bool>("performanceOutput")) performanceOutput.show();

    if (!Options::get<bool>("animatedBackground")) animatedBackground.disable();

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
    elements.push_back(&trainingUI);

    player_popup.setBoundery({0, 0, 380, 600});
    player_popup.add("Set Handicap")
        .add("0%", []() { TaskQueue::add(Task::NotDuringRound, []() { SendSignal(23, 0); }); })
        .add("10%", []() { TaskQueue::add(Task::NotDuringRound, []() { SendSignal(23, 10); }); })
        .add("20%", []() { TaskQueue::add(Task::NotDuringRound, []() { SendSignal(23, 20); }); })
        .add("30%", []() { TaskQueue::add(Task::NotDuringRound, []() { SendSignal(23, 30); }); })
        .add("40%", []() { TaskQueue::add(Task::NotDuringRound, []() { SendSignal(23, 40); }); })
        .add("50%", []() { TaskQueue::add(Task::NotDuringRound, []() { SendSignal(23, 50); }); })
        .add("60%", []() { TaskQueue::add(Task::NotDuringRound, []() { SendSignal(23, 60); }); })
        .add("70%", []() { TaskQueue::add(Task::NotDuringRound, []() { SendSignal(23, 70); }); })
        .add("80%", []() { TaskQueue::add(Task::NotDuringRound, []() { SendSignal(23, 80); }); })
        .add("90%", []() { TaskQueue::add(Task::NotDuringRound, []() { SendSignal(23, 90); }); });
    player_popup.add("Away", []() { Away(); }).add("Ready", []() { Ready(); }).update();

    connectSignal("Show", [&](int elem) { elements[elem]->show(); });
    connectSignal("Hide", [&](int elem) { elements[elem]->hide(); });
    connectSignal("Enable", [&](int elem) { elements[elem]->enable(); });
    connectSignal("Disable", [&](int elem) { elements[elem]->disable(); });
    connectSignal("IsVisible", [&](int elem) { return elements[elem]->isVisible(); });
    connectSignal("QuickMsg", [&](const std::string& msg) {
        QuickMsg.text(msg);
        QuickMsg.show();
        quickMsgTime = resources.delayClock.getElapsedTime();
    });
    connectSignal("Disconnect", [&](int showMsg) {
        if (showMsg == 2) return;
        resources.playonline = false;
        udpConfirmed = false;
        performanceOutput.ping.hide();
        onlineplayUI.matchButton->setText("Join 1vs1 matchmaking");
        serverUI.clear();
        chatScreen.hide();
        onlineplayUI.hide();
        areYouSure.hide();
        loginBox.connectingScreen.hide();
        challengesGameUI.hide();
        replayUI.hide();
        SetGameState(GameStates::MainMenu);
        trainingUI.hide();
        mainMenu.show();
        if (showMsg) QuickMSG("Disconnected from server");
    });

    PM::handle_packet([&](const NP_Welcome& p) {
        resources.myId = p.yourID;
        serverUI.motd->setText(p.message);
        performanceOutput.ping.show();
    });
    PM::handle_packet([&](const NP_ClientJoinedRoom& p) {
        AddField(p.player.id, p.player.name);
        if (gameStandings.isVisible()) gameStandings.alignResult();
    });
    PM::handle_packet([&](const NP_ChallengeNotRecord& p) { QuickMSG("Your score of " + p.message); });

    PM::handle_packet([&](const NP_ClientLeftRoom& p) {
        gameFieldDrawer.removeField(p.id);
        if (gameStandings.isVisible()) gameStandings.alignResult();
    });

    PM::handle_packet([&](const NP_AuthResult& p) { getAuthResult(p); });
    PM::handle_packet<NP_UdpConfirmed>([&]() { udpConfirmed = true; });
    PM::handle_packet<NP_ClientJoinedMatchmaking>([&]() { onlineplayUI.matchButton->setText("Leave 1vs1 matchmaking"); });
    PM::handle_packet<NP_ClientLeftMatchmaking>([&]() { onlineplayUI.matchButton->setText("Join 1vs1 matchmaking"); });
    PM::handle_packet<NP_ClientRemovedMatchmaking>([&]() {
        QuickMSG("You were removed from the matchmaking queue");
        onlineplayUI.matchButton->setText("Join 1vs1 matchmaking");
    });
    PM::handle_packet([&](const NP_Kick& p) {
            QuickMSG("Kicked: " + p.message);
        SetGameState(GameStates::MainMenu);
    });
}

void GuiElements::getAuthResult(const NP_AuthResult& p) {
    if (loginBox.t.joinable()) loginBox.t.join();
    if (p.status == 1) {
        resources.name = p.name;
        resources.myId = p.id;
        SetName(resources.name);
        loginBox.connectingScreen.hide();
        onlineplayUI.show();
        onlineplayUI.opTab->select(0);
    } else if (p.status == 2) {
        loginBox.connectingScreen.hide();
        onlineplayUI.show();
        onlineplayUI.opTab->select(0);
    } else {
        if (p.status == 3) {
            Disconnect(2);
            loginBox.connectingScreen.label->setText("You have the wrong client version, attempting to patch...");
            performanceOutput.ping.hide();
            loginBox.t = std::thread(&PatchCheck::check, &loginBox.patcher, resources.clientVersion);
            return;
        } else if (p.status == 4)
            QuickMSG("Name already in use");
        else
            QuickMSG("Authentication failed");
        Disconnect(0);
        loginBox.connectingScreen.hide();
        mainMenu.show();
        loginBox.show();
        trainingUI.hide();
        performanceOutput.ping.hide();
    }
}

void GuiElements::delayCheck(const sf::Time& currentTime) {
    if (QuickMsg->isVisible())
        if (currentTime - quickMsgTime > sf::seconds(5)) QuickMsg.hide();

    if (loginBox.connectingScreen.isVisible()) loginBox.checkStatus();

    if (resources.playonline) {
        if (onlineplayUI.isVisible()) {
            if (onlineplayUI.roomList.isVisible())
                if (currentTime - onlineplayUI.updateRoomListTime > sf::seconds(5)) {
                    onlineplayUI.updateRoomListTime = currentTime;
                    SendSignal(16);
                }
            if (onlineplayUI.tournamentList.isVisible())
                if (currentTime - onlineplayUI.updateTournamentListTime > sf::seconds(5)) {
                    onlineplayUI.updateTournamentListTime = currentTime;
                    SendSignal(15);
                }
        }
    }

    slideMenu.update(currentTime);

    if (bugReport.join)
        if (bugReport.t.joinable()) bugReport.t.join();

    chatScreen.fade(currentTime);
}

void GuiElements::toggleFullscreen() {
    if (!Options::get<bool>("fullscreen")) {
        if (Options::get<short>("currentmode") == -1) Options::get<short>("currentmode") = 0;
        resources.window.close();
        resources.window.create(Options::get_videomodes()[Options::get<short>("currentmode")], "SpeedBlocks", sf::Style::Fullscreen);
        resources.window.setView(sf::View(sf::FloatRect(0, 0, 960, 600)));
        Options::get<bool>("fullscreen") = true;
    } else if (Options::get<bool>("fullscreen") || !resources.window.isOpen()) {
        resources.window.close();
        resources.window.create(sf::VideoMode(960, 600), "SpeedBlocks");
        resources.window.setView(sf::View(sf::FloatRect(0, 0, 960, 600)));
        Options::get<bool>("fullscreen") = false;
    }
}

bool GuiElements::keyEvents(sf::Event& event) {
    if (event.type == sf::Event::KeyPressed) {
        if (chatScreen.isActive()) chatScreen.focus();
        if (event.key.code == sf::Keyboard::Escape) {
            if (areYouSure.isVisible())
                areYouSure.hide();
            else if (chatScreen.isActive())
                chatScreen.deactivate();
            else if (mainMenu.isVisible()) {
                areYouSure.label->setText("Do you want to quit?");
                areYouSure.show();
            } else if (onlineplayUI.isVisible()) {
                areYouSure.label->setText("Leave the server?");
                areYouSure.show();
            } else if (challengesGameUI.isVisible()) {
                areYouSure.label->setText("Leave this challenge?");
                areYouSure.show();
            } else if (resources.gamestate != GameStates::MainMenu) {
                areYouSure.label->setText("Leave this game?");
                areYouSure.show();
            } else if (trainingUI.isVisible()) {
                trainingUI.hide();
                mainMenu.show();
            }
        } else if (event.key.code == sf::Keyboard::Return) {
            if (event.key.alt)
                toggleFullscreen();
            else if (areYouSure.isVisible())
                areYouSure.ausY();
            else if (resources.playonline && !chatScreen.isActive()) {
                chatScreen.activate();
                return true;
            }
        } else if (event.key.code == sf::Keyboard::Tab && chatScreen.isActive()) {
            chatScreen.toggleSendTo();
            return true;
        }
    }
    return false;
}

void GuiElements::mouseEvents(sf::Event& event) {
    if (event.type == sf::Event::MouseButtonPressed) {
        if (event.mouseButton.button == sf::Mouse::Right) {
            if (resources.gamestate != GameStates::MainMenu && resources.playonline) player_popup.showIfInside({event.mouseButton.x, event.mouseButton.y});
        }
    }
}

void GuiElements::windowEvents(sf::Event& event) {
    if (event.type == sf::Event::Closed)
        resources.window.close();
    else if (event.type == sf::Event::Resized && !Options::get<bool>("fullscreen"))
        resizeWindow(event);
    else if (event.type == sf::Event::LostFocus)
        resources.window.setFramerateLimit(60);
    else if (event.type == sf::Event::GainedFocus)
        resources.window.setFramerateLimit(0);
}

void GuiElements::resizeWindow(sf::Event& event) {
    sf::View view = resources.window.getView();
    float ratio;
    if ((float)event.size.width / event.size.height > 960.0 / 600) {
        ratio = (event.size.height * (960.0 / 600)) / event.size.width;
        view.setViewport(sf::FloatRect((1 - ratio) / 2.0, 0, ratio, 1));
    } else {
        ratio = (event.size.width / (960.0 / 600)) / event.size.height;
        view.setViewport(sf::FloatRect(0, (1 - ratio) / 2.0, 1, ratio));
    }
    resources.window.setView(view);
    resources.gfx->tGui.setView(view);
    resources.window.clear(sf::Color(0, 0, 0));
}

bool GuiElements::handleEvent(sf::Event& event) {
    windowEvents(event);

    if (gameOptions.SelectKey->isVisible())
        if (gameOptions.putKey(event)) return true;

    if (keyEvents(event)) return true;

    mouseEvents(event);

    if (gameFieldDrawer.isVisible()) gameFieldDrawer.enlargePlayfield(event);

    if (onlineplayUI.isVisible()) {
        onlineplayUI.roomList.scrolled(event);
        onlineplayUI.tournamentList.scrolled(event);
        onlineplayUI.challengesUI.challengeList.scrolled(event);
    }

    slideMenu.handleEvent(event);
    return false;
}
