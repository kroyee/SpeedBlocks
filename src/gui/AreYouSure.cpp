#include "gui.h"
#include "AreYouSure.h"

void AreYouSure::create(sf::Rect<int> _pos, UI* _gui) {
	createBase(_pos, _gui);
	panel->setBackgroundColor(sf::Color(255,255,255,0));

	tgui::Panel::Ptr box = gui->themeTG->load("Panel");
	box->setPosition(330,250);
	box->setSize(300,100);
	panel->add(box);

	label = gui->themeTG->load("Label");
	label->setPosition(0, 20);
	label->setSize(300, 50);
	label->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Center);
	label->setText("Are you sure?");
	box->add(label);

	tgui::Button::Ptr AUSYB = gui->themeTG->load("Button");
	AUSYB->setText("Yes");
	AUSYB->setPosition(50, 60);
	AUSYB->setSize(75, 30);
	AUSYB->setOpacity(0.7);
	AUSYB->connect("Pressed", &AreYouSure::ausY, this);
	box->add(AUSYB);

	tgui::Button::Ptr AUSNB = gui->themeTG->load("Button");
	AUSNB->setText("No");
	AUSNB->setPosition(175, 60);
	AUSNB->setSize(75, 30);
	AUSNB->setOpacity(0.7);
	AUSNB->connect("Pressed", &AreYouSure::ausN, this);
	box->add(AUSNB);
}

void AreYouSure::ausY() {
	if (gui->playonline) {
		if (gui->gamestate != MainMenu)
			gui->leaveRoom();
		else {
			((guiBase*)gui->onlineplayUI)->hide();
			((guiBase*)gui->mainMenu)->show();
			gui->disconnect();
		}
	}
	else if (gui->gamestate == MainMenu)
		gui->window->close();
	else
		gui->setGameState(MainMenu);
	hide();
}

void AreYouSure::ausN() {
	hide();
	((guiBase*)gui->mainMenu)->enable();
	((guiBase*)gui->onlineplayUI)->enable();
}