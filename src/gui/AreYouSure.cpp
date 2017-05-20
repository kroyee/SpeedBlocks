#include "MainMenu.h"
#include "gui.h"
#include "gamePlay.h"

void AreYouSure::create(sf::Rect<int> _pos, UI* _gui) {
	createBase(_pos, _gui);
	panel->setBackgroundColor(sf::Color(50,50,50,200));

	label = gui->themeTG->load("Label");
	label->setPosition(0, 20);
	label->setSize(300, 50);
	label->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Center);
	label->setText("Are you sure?");
	panel->add(label);

	tgui::Button::Ptr AUSYB = gui->themeBB->load("Button");
	AUSYB->setText("Yes");
	AUSYB->setPosition(50, 60);
	AUSYB->setSize(75, 30);
	AUSYB->setOpacity(0.7);
	AUSYB->connect("Pressed", &AreYouSure::ausY, this);
	panel->add(AUSYB);

	tgui::Button::Ptr AUSNB = gui->themeBB->load("Button");
	AUSNB->setText("No");
	AUSNB->setPosition(175, 60);
	AUSNB->setSize(75, 30);
	AUSNB->setOpacity(0.7);
	AUSNB->connect("Pressed", &AreYouSure::ausN, this);
	panel->add(AUSNB);
}

void AreYouSure::ausY() {
	if (gui->playonline)
		gui->leaveRoom();
	else if (gui->gamestate == MainMenu)
		gui->window->close();
	else
		gui->setGameState(MainMenu);
	hide();
}

void AreYouSure::ausN() {
	hide();
	gui->mainMenu.enable();
}