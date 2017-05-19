#include "MainMenu.h"
#include "gui.h"
#include "gamePlay.h"

void AreYouSure::create(sf::Rect<int> _pos, UI* _ui) {
	createBase(_pos, _ui);
	panel->setBackgroundColor(sf::Color(50,50,50,200));

	label = ui->themeTG->load("Label");
	label->setPosition(0, 20);
	label->setSize(300, 50);
	label->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Center);
	label->setText("Are you sure?");
	panel->add(label);

	tgui::Button::Ptr AUSYB = ui->themeBB->load("Button");
	AUSYB->setText("Yes");
	AUSYB->setPosition(50, 60);
	AUSYB->setSize(75, 30);
	AUSYB->setOpacity(0.7);
	AUSYB->connect("Pressed", &AreYouSure::ausY, this);
	panel->add(AUSYB);

	tgui::Button::Ptr AUSNB = ui->themeBB->load("Button");
	AUSNB->setText("No");
	AUSNB->setPosition(175, 60);
	AUSNB->setSize(75, 30);
	AUSNB->setOpacity(0.7);
	AUSNB->connect("Pressed", &AreYouSure::ausN, this);
	panel->add(AUSNB);
}

void AreYouSure::ausY() {
	if (ui->playonline)
		ui->leaveRoom();
	else if (ui->gamestate == MainMenu)
		ui->window->close();
	else
		ui->setGameState(MainMenu);
	hide();
}

void AreYouSure::ausN() {
	hide();
	ui->mainMenu.enable();
}