#include "AreYouSure.h"
#include <iostream>
#include "GameSignals.h"
#include "Resources.h"

static auto& LeaveRoom = Signal<void>::get("LeaveRoom");
static auto& Hide = Signal<void, int>::get("Hide");
static auto& Disconnect = Signal<void, int>::get("Disconnect");
static auto& Show = Signal<void, int>::get("Show");
static auto& SetGameState = Signal<void, GameStates>::get("SetGameState");

AreYouSure::AreYouSure(sf::Rect<int> _pos, Resources& _res) : GuiBase(_pos, _res) {
    os::Panel box();
    box.pos(330, 250).size(300, 100).add_to(panel).background();

    label.text("Are you sure?").size(300, 40).add_to(panel).pos(0, 20)->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Center);

    Button().text("Yes").size(75, 30).pos(50, 60).add_to(panel).connect("Pressed", &AreYouSure::ausY, this);

    Button().text("No").size(75, 30).pos(130, 60).add_to(panel).connect("Pressed", &AreYouSure::ausN, this);

    connectSignal("SetAreYouSure", &AreYouSure::setAUS, this);
}

void AreYouSure::ausY() {
    if (resources.playonline) {
        if (resources.gamestate != GameStates::MainMenu)
            LeaveRoom();
        else {
            Hide(4);
            Disconnect(0);
            Hide(16);
            Show(0);
        }
    } else if (resources.gamestate == GameStates::MainMenu)
        resources.window.close();
    else
        SetGameState(GameStates::MainMenu);
    hide();
}

void AreYouSure::ausN() { hide(); }

void AreYouSure::setAUS(const std::string& text) {
    label.text(text);
    show();
}
