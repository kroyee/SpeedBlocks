#include "AreYouSure.h"
#include "GameSignals.h"
#include <iostream>

static auto& LeaveRoom = Signal<void>::get("LeaveRoom");
static auto& Hide = Signal<void, int>::get("Hide");
static auto& Disconnect = Signal<void, int>::get("Disconnect");
static auto& Show = Signal<void, int>::get("Show");
static auto& SetGameState = Signal<void, GameStates>::get("SetGameState");

AreYouSure::AreYouSure(sf::Rect<int> _pos, Resources& _res) : guiBase(_pos, _res) {
	tgui::Panel::Ptr box = loadWidget("Panel", {330, 250, 300, 100});

	label = loadWidgetTo(box, "Label", {0, 20, 300, 50}, "Are you sure?");
	label->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Center);

	tgui::Button::Ptr AUSYB = loadWidgetTo(box, "Button", {50, 60, 75, 30}, "Yes");
	AUSYB->setOpacity(0.7);
	AUSYB->connect("Pressed", &AreYouSure::ausY, this);

	tgui::Button::Ptr AUSNB = loadWidgetTo(box, "Button", {175, 60, 75, 30}, "No");
	AUSNB->setOpacity(0.7);
	AUSNB->connect("Pressed", &AreYouSure::ausN, this);

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
	}
	else if (resources.gamestate == GameStates::MainMenu)
		resources.window.close();
	else
		SetGameState(GameStates::MainMenu);
	hide();
}

void AreYouSure::ausN() {
	hide();
}

void AreYouSure::setAUS(const std::string& text) {
	label->setText(text);
	show();
}