#include "MainMenu.h"
#include "gui.h"
#include "gamePlay.h"

void Menu::create(sf::Rect<int> _pos, UI* _ui) {
	createBase(_pos, _ui);

	tgui::Button::Ptr PO = ui->themeTG->load("Button");
	PO->setSize(480, 100);
	PO->setPosition(240, 100);
	PO->setText("Play Online");
	PO->setTextSize(72);
	PO->setFont(ui->resources.gfx.typewriter);
	PO->connect("pressed", &Menu::playOnline, this);
	panel->add(PO);

	tgui::Button::Ptr Tr = ui->themeTG->load("Button");
	Tr->setSize(360, 100);
	Tr->setPosition(300, 220);
	Tr->setText("Training");
	Tr->setTextSize(72);
	Tr->setFont(ui->resources.gfx.typewriter);
	Tr->connect("pressed", &Menu::Training, this);
	panel->add(Tr);

	tgui::Button::Ptr Opt = ui->themeTG->load("Button");
	Opt->setSize(310, 100);
	Opt->setPosition(325, 340);
	Opt->setText("Options");
	Opt->setTextSize(72);
	Opt->setFont(ui->resources.gfx.typewriter);
	Opt->connect("pressed", &Menu::Options, this);
	panel->add(Opt);

	tgui::Button::Ptr Quit = ui->themeTG->load("Button");
	Quit->setSize(180, 100);
	Quit->setPosition(390, 460);
	Quit->setText("Quit");
	Quit->setTextSize(72);
	Quit->setFont(ui->resources.gfx.typewriter);
	Quit->connect("pressed", &Menu::quitGame, this);
	panel->add(Quit);
}

void Menu::playOnline() {
	disable();
	ui->loginBox.open();
}

void Menu::Training() {
	ui->setGameState(CountDown);
    ui->game.startCountdown();
}

void Menu::Options() {
	hide();
	ui->gameOptions.show();
	ui->gameOptions.otab->select(0);
}

void Menu::quitGame() {
	ui->window->close();
}