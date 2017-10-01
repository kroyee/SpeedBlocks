#include "AreYouSure.h"
#include "Signal.h"
#include <iostream>

AreYouSure::AreYouSure(sf::Rect<int> _pos, Resources& _res) : guiBase(_pos, _res) {
	panel->setBackgroundColor(sf::Color(255,255,255,0));

	tgui::Panel::Ptr box = resources.gfx->themeTG->load("Panel");
	box->setPosition(330,250);
	box->setSize(300,100);
	panel->add(box);

	label = resources.gfx->themeTG->load("Label");
	label->setPosition(0, 20);
	label->setSize(300, 50);
	label->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Center);
	label->setText("Are you sure?");
	box->add(label);

	tgui::Button::Ptr AUSYB = resources.gfx->themeTG->load("Button");
	AUSYB->setText("Yes");
	AUSYB->setPosition(50, 60);
	AUSYB->setSize(75, 30);
	AUSYB->setOpacity(0.7);
	AUSYB->connect("Pressed", &AreYouSure::ausY, this);
	box->add(AUSYB);

	tgui::Button::Ptr AUSNB = resources.gfx->themeTG->load("Button");
	AUSNB->setText("No");
	AUSNB->setPosition(175, 60);
	AUSNB->setSize(75, 30);
	AUSNB->setOpacity(0.7);
	AUSNB->connect("Pressed", &AreYouSure::ausN, this);
	box->add(AUSNB);

	Signals::SetAreYouSure.connect(&AreYouSure::setAUS, this);
}

void AreYouSure::ausY() {
	if (resources.playonline) {
		if (resources.gamestate != GameStates::MainMenu)
			Signals::LeaveRoom();
		else {
			Signals::Hide(4);
			Signals::Show(0);
			Signals::Disconnect(0);
		}
	}
	else if (resources.gamestate == GameStates::MainMenu)
		resources.window.close();
	else
		Signals::SetGameState(GameStates::MainMenu);
	hide();
}

void AreYouSure::ausN() {
	hide();
}

void AreYouSure::setAUS(const sf::String& text) {
	label->setText(text);
	show();
}