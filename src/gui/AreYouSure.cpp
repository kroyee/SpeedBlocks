#include "AreYouSure.h"
#include "GameSignals.h"
#include "Resources.h"
#include <iostream>

static auto& LeaveRoom = Signal<void>::get("LeaveRoom");
static auto& Hide = Signal<void, int>::get("Hide");
static auto& Disconnect = Signal<void, int>::get("Disconnect");
static auto& Show = Signal<void, int>::get("Show");
static auto& SetGameState = Signal<void, GameStates>::get("SetGameState");

AreYouSure::AreYouSure(sf::Rect<int> _pos, Resources& _res) : GuiBase(_pos, _res) {
	tgui::Panel::Ptr box = panel2({330, 250, 300, 100});
	panel->add(box);

	label = label1("Are you syre?");
	label->setSize(300, 40);

	auto yes = button1("Yes");
	yes->connect("Pressed", &AreYouSure::ausY, this);

	auto no = button1("No");
	no->connect("Pressed", &AreYouSure::ausN, this);

	alignOn<TextAlign<tgui::Label::HorizontalAlignment::Center>, Size<300, 40>>(box, 0,20) << label;
	alignOn<Size<75, 30>>(box, 50, 60, 50, false) << yes << no;

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
