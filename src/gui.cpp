#include "gui.h"
#include "GamePlay.h"
#include "GameSignals.h"
#include "GuiElements.h"
#include "NetworkPackets.hpp"
#include "Options.h"
#include "TaskQueue.h"
#include "Textures.h"
#include "UIGameState.h"

#ifdef __APPLE__
#include "ResourcePath.hpp"
#else
#include "EmptyResourcePath.h"
#endif

static auto& QuickMsg = Signal<void, const std::string&>::get("QuickMsg");
static auto& SeedRander = Signal<void, int, int>::get("SeedRander");
static auto& StartCountDown = Signal<void>::get("StartCountDown");
static auto& SetGameState = Signal<void, GameStates>::get("SetGameState");
static auto& GameSetup = Signal<void, int>::get("GameSetup");
static auto& GameOver = Signal<void, int>::get("GameOver");
static auto& AddField = Signal<ObsField&, int, const std::string&>::get("AddField");
static auto& GameClear = Signal<void>::get("GameClear");
static auto& SetDrawMe = Signal<void>::get("SetDrawMe");

UI::UI(sf::RenderWindow& window_, GamePlay& game_)
    : resources(game_.resources),
      game(game_),
      guiElements(new GuiElements(resources)),
      window(&window_),
      away(false),
      delayClock(resources.delayClock),
      gamestate(game_.resources.gamestate),
      state(std::unique_ptr<UIBaseState>(new UIMainMenu(*this))),
      myId(resources.myId) {
    guiElements->trainingUI.hide();

    connectSignal("LeaveRoom", &UI::leaveRoom, this);
    connectSignal("SetGameState", [&](GameStates _state) { UIBaseState::set(state, _state); });
    connectSignal("LightTheme", &UI::lightTheme, this);
    connectSignal("DarkTheme", &UI::darkTheme, this);
    connectSignal("JoinRoom", &UI::joinRoom, this);

    PM::handle_packet([&](const NP_Gamestate& p) { getGameState(p); });
    PM::handle_packet([&](const NP_Replay& p) { receiveRecording(p); });

    PM::handle_packet([&](const NP_Ping& p) { guiElements->performanceOutput.setPing(ping.get(delayClock.getElapsedTime(), p)); });
    PM::handle_packet([&](const NP_Countdown& p) {
        countdown.set(delayClock.getElapsedTime(), p.countdown);
        if (countdown.ongoing() && !away && gamestate != GameStates::CountDown) {
            SetGameState(GameStates::CountDown);
            game.startCountdown();
        }
    });

    PM::handle_packet([&](const NP_JoinResponse& p) { joinRoomResponse(p); });

    PM::handle_packet<NP_TournamentNotEnough>([&]() { QuickMsg("Not enough players to start tournament"); });
    PM::handle_packet<NP_NotAsGuest>([&]() { QuickMsg("You can't do that as guest, register at https://speedblocks.se"); });
    PM::handle_packet([&](const NP_CountdownStart& p) {
        SeedRander(p.seed1, p.seed2);
        StartCountDown();
        guiElements->gameFieldDrawer.resetOppFields();
        SetGameState(GameStates::CountDown);
        countdown.start(delayClock.getElapsedTime());
        if (guiElements->challengesGameUI.isVisible()) {
            guiElements->challengesGameUI.clear();
            if (guiElements->challengesGameUI.challenge->type == Challenges::Cheese)
                GameSetup(1);
            else if (guiElements->challengesGameUI.challenge->type == Challenges::Cheese30L)
                GameSetup(2);
        }
    });
    PM::handle_packet([&](const NP_CountdownStop& p) {
        if (!p.status && gamestate == GameStates::Game) GameOver(0);
        countdown.stop();
    });
    PM::handle_packet<NP_RoundEnd>([&]() {
        if (gamestate != GameStates::Practice) GameOver(0);
        countdown.stop();
    });
    PM::handle_packet<NP_YouWon>([&]() {
        GameOver(1);
        countdown.stop();
    });
    PM::handle_packet([&](const NP_RoundStart& p) {
        guiElements->gameFieldDrawer.resetOppFields();
        SeedRander(p.seed1, p.seed2);
        if (gamestate != GameStates::Spectating) {
            GameClear();
            SetDrawMe();
        }
        countdown.start(delayClock.getElapsedTime());
    });
    PM::handle_packet<NP_NotHero>([&]() { QuickMsg("You need to reach rank 0 to join the Hero room"); });
    PM::handle_packet<NP_ClientStillInMatchmaking>([&]() { QuickMsg("You are still in the matchmaking queue"); });

    if (Options::get<uint8_t>("theme") == 1)
        lightTheme();
    else
        darkTheme();

    setOnChatFocus(resources.gfx->tGui.getWidgets());
}

UI::~UI() { delete guiElements; }

void UI::joinRoom(int id) {
    SendSignal(0, id);
    away = false;
    game.autoaway = false;
}

void UI::leaveRoom() {
    SendSignal(1);
    SetGameState(GameStates::MainMenu);
}

void UI::chatFocus(bool i) {
    if (i) {
        resources.chatFocused = true;
        window->setKeyRepeatEnabled(true);
    } else {
        resources.chatFocused = false;
        window->setKeyRepeatEnabled(false);
    }
}

void UI::receiveRecording(const NP_Replay& p) {
    game.recorder.receiveRecording(p);
    SetGameState(GameStates::Replay);
    if (p.type >= 20000) {
        guiElements->gameFieldDrawer.hide();
        guiElements->challengesGameUI.openChallenge(p.type);
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
                PM::write_udp(NP_ConfirmUdp{myId});
            }

        if (gamestate == GameStates::CountDown)
            if (game.countDown(countdown.check(currentTime))) SetGameState(GameStates::Game);

        guiElements->performanceOutput.setPing(ping.send(currentTime, myId));
    }

    TaskQueue::perform(Task::MainThread);
    if (resources.gamestate != GameStates::Game) TaskQueue::perform(Task::NotDuringRound);
}

bool UI::handleEvent(sf::Event& event) {
    if (guiElements->handleEvent(event)) return false;

    resources.gfx->tGui.handleEvent(event);
    return true;
}

void UI::darkTheme() {
    guiElements->animatedBackground.dark();
    guiElements->slideMenu.dark();
    guiElements->chatScreen.dark();
    game.field.text.setColor(sf::Color(255, 255, 255));
    game.pressEnterText.setFillColor(sf::Color(255, 255, 255));
    game.pressEnterText.setOutlineColor(sf::Color(255, 255, 255));
    game.drawMe = true;
    setWidgetTextColor(sf::Color(255, 255, 255, 200));
    // Panel_renderer.setBackgroundColor(color);
    Options::get<uint8_t>("theme") = 2;
    for (auto& field : guiElements->gameFieldDrawer.fields) field.text.setColor(sf::Color(255, 255, 255));
}

void UI::lightTheme() {
    guiElements->animatedBackground.light();
    guiElements->slideMenu.light();
    guiElements->chatScreen.light();
    game.field.text.setColor(sf::Color(0, 0, 0));
    game.pressEnterText.setFillColor(sf::Color(0, 0, 0));
    game.pressEnterText.setOutlineColor(sf::Color(0, 0, 0));
    game.drawMe = true;
    setWidgetTextColor(sf::Color(0, 0, 0, 200));
    // Panel_renderer.setBackgroundColor(color);
    Options::get<uint8_t>("theme") = 1;
    for (auto& field : guiElements->gameFieldDrawer.fields) field.text.setColor(sf::Color(0, 0, 0));

    /*tgui::Texture texture = tgui::Deserializer::deserialize(tgui::ObjectConverter::Type::Texture, resources.gfx->theme1->getProperty("Button",
    "DownImage")).getTexture(); texture.setColor(sf::Color(255, 0, 0, 115)); Button_renderer.setDownImage(color);*/
}

void UI::setWidgetTextColor(sf::Color color) {
    tgui::LabelRenderer(resources.gfx->theme1.getRenderer("Label")).setTextColor(color);

    auto button_renderer = tgui::ButtonRenderer(resources.gfx->theme1.getRenderer("Button"));
    color.a = 215;
    button_renderer.setTextColor(color);
    color.a = 235;
    button_renderer.setTextColorHover(color);
    button_renderer.setTextColorDown(color);

    auto checkbox_renderer = tgui::RadioButtonRenderer(resources.gfx->theme1.getRenderer("CheckBox"));

    color.a = 215;
    checkbox_renderer.setTextColor(color);
    color.a = 235;
    checkbox_renderer.setTextColorHover(color);
    color.a = 180;
    checkbox_renderer.setCheckColor(color);
    color.a = 200;
    checkbox_renderer.setCheckColorHover(color);

    auto EditBox_renderer = tgui::EditBoxRenderer(resources.gfx->theme1.getRenderer("EditBox"));
    color.a = 180;
    EditBox_renderer.setTextColor(color);
    color.a = 225;
    EditBox_renderer.setSelectedTextColor(color);
    color.a = 180;
    EditBox_renderer.setDefaultTextColor(color);
    color.a = 215;
    EditBox_renderer.setCaretColor(color);

    auto ListBox_renderer = tgui::ListBoxRenderer(resources.gfx->theme1.getRenderer("ListBox"));
    color.a = 245;
    ListBox_renderer.setSelectedTextColor(color);
    color.a = 215;
    ListBox_renderer.setTextColor(color);
    color.a = 235;
    ListBox_renderer.setTextColorHover(color);

    auto Tab_renderer = tgui::TabsRenderer(resources.gfx->theme1.getRenderer("Tabs"));
    color.a = 215;
    Tab_renderer.setTextColor(color);
    color.a = 245;
    Tab_renderer.setSelectedTextColor(color);

    auto RadioButton_renderer = tgui::RadioButtonRenderer(resources.gfx->theme1.getRenderer("RadioButton"));
    color.a = 215;
    RadioButton_renderer.setTextColor(color);
    color.a = 235;
    RadioButton_renderer.setTextColorHover(color);
    color.a = 180;
    RadioButton_renderer.setCheckColor(color);
    color.a = 200;
    RadioButton_renderer.setCheckColorHover(color);

    auto TextBox_renderer = tgui::TextBoxRenderer(resources.gfx->theme1.getRenderer("TextBox"));
    color.a = 180;
    TextBox_renderer.setTextColor(color);
    color.a = 225;
    TextBox_renderer.setSelectedTextColor(color);
    color.a = 215;
    TextBox_renderer.setCaretColor(color);
}

void UI::setOnChatFocus(const std::vector<tgui::Widget::Ptr> widgets) {
    for (auto& widget : widgets) {
        if (widget->getWidgetType() == "Panel")
            setOnChatFocus(std::static_pointer_cast<tgui::Panel>(widget)->getWidgets());
        else if (widget->getWidgetType() == "EditBox" || widget->getWidgetType() == "TextBox")
            widget->connect({"Focused", "Unfocused"}, [&, widget]() { chatFocus(widget->isFocused()); });
    }
}

void UI::getGameState(const NP_Gamestate& p) {
    for (auto&& field : guiElements->gameFieldDrawer.fields)
        if (field.id == p.id) {
            if (p.count > field.datacount || (p.count < 50 && field.datacount > 200)) {
                field.datacount = p.count;
                resources.compressor->loadTmp(p.data);
                resources.compressor->extract();
                if (resources.compressor->validate()) {
                    resources.compressor->field = &field;
                    resources.compressor->copy();
                    field.drawMe = true;
                }
            }
            break;
        }
}

void UI::joinRoomResponse(const NP_JoinResponse& p) {
    auto joinok = p.status;
    if (joinok == 1 || joinok == 1000) {
        SeedRander(p.seed1, p.seed2);

        if (joinok == 1) {
            SetGameState(GameStates::GameOver);
            game.pressEnterText.setString("press P to start practice");
            game.field.clear();
            game.drawMe = true;
            guiElements->gameFieldDrawer.setPosition(465, 40);
            guiElements->gameFieldDrawer.setSize(450, 555);
        } else {
            SetGameState(GameStates::Spectating);
            guiElements->gameFieldDrawer.setPosition(5, 40);
            guiElements->gameFieldDrawer.setSize(910, 555);
        }

        std::string name;
        for (auto& player : p.players) {
            AddField(player.id, player.name);
        }
        if (gamestate == GameStates::Spectating) guiElements->gameStandings.alignResult();

        guiElements->chatScreen.sendTo("Room");
    } else if (joinok == 2)
        QuickMsg("Room is full");
    else if (joinok == 3)
        QuickMsg("Please wait for server to get your user-data");
    else if (joinok == 4)
        QuickMsg("This is not your game");
    else if (joinok >= 20000) {
        SetGameState(GameStates::GameOver);
        guiElements->gameFieldDrawer.hide();
        game.pressEnterText.setString("press P to start challenge");
        game.field.clear();
        game.drawMe = true;
        guiElements->challengesGameUI.openChallenge(joinok);
    }
}
