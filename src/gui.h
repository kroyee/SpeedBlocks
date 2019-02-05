#ifndef GUI_H
#define GUI_H

#include <TGUI/TGUI.hpp>
#include <functional>
#include <memory>
#include "CountdownHandle.h"
#include "GameFieldDrawer.h"
#include "PingHandle.h"
#include "Resources.h"

class soundBank;
class GamePlay;
class network;
class textures;
class Resources;
struct GuiElements;
class ObsField;
class UIBaseState;
struct NP_Gamestate;
struct NP_JoinResponse;
struct NP_Replay;

class UI {
   public:
    UI(sf::RenderWindow& window_, GamePlay& game_);
    ~UI();

    Resources& resources;
    GamePlay& game;
    PingHandle ping;
    CountdownHandle countdown;

    GuiElements* guiElements;

    sf::RenderWindow* window;

    bool away;

    sf::Time quickMsgTime;
    sf::Time udpPortTime;
    sf::Clock& delayClock;

    GameStates& gamestate;
    std::unique_ptr<UIBaseState> state;

    uint16_t& myId;

    void joinRoom(int);
    void leaveRoom();

    void chatFocus(bool i);

    void receiveRecording(const NP_Replay&);

    void getAlert();

    bool handleEvent(sf::Event& event);

    void getGameState(const NP_Gamestate&);

    void delayCheck();

    void darkTheme();
    void lightTheme();
    void setWidgetTextColor(sf::Color color);

    void setOnChatFocus(const std::vector<tgui::Widget::Ptr> widgets);

    void joinRoomResponse(const NP_JoinResponse&);
};

#endif
