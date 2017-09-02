#include "MainMenu.h"
#include "gui.h"
#include "gamePlay.h"
#include "LoginBox.h"
#include "GameOptions.h"
#include "GameStandings.h"

void Menu::create(sf::Rect<int> _pos, UI* _gui) {
	createBase(_pos, _gui);

	tgui::Picture::Ptr logo = tgui::Picture::create(gui->resources.gfx.logoTexture);
	logo->setPosition(0, -20);
	panel->add(logo);

	tgui::Button::Ptr Tr = gui->themeTG->load("Button");
	Tr->setSize(340, 80);
	Tr->setPosition(60, 300);
	Tr->setText("Training");
	Tr->setTextSize(68);
	Tr->setFont(gui->resources.gfx.typewriter);
	Tr->connect("pressed", &Menu::Training, this);
	panel->add(Tr);

	tgui::Button::Ptr Quit = gui->themeTG->load("Button");
	Quit->setSize(190, 80);
	Quit->setPosition(135, 420);
	Quit->setText("Quit");
	Quit->setTextSize(68);
	Quit->setFont(gui->resources.gfx.typewriter);
	Quit->connect("pressed", &Menu::quitGame, this);
	panel->add(Quit);

	tgui::Label::Ptr version = gui->themeTG->load("Label");
	version->setPosition(5,575);
	int version_patch = (gui->clientVersion % 10000) % 100;
	int version_minor = ((gui->clientVersion - version_patch) % 10000) / 100;
	int version_major = (gui->clientVersion - version_minor - version_patch) / 10000;
	version->setText("v"+std::to_string(version_major)+"."+std::to_string(version_minor)+"."+std::to_string(version_patch));
	panel->add(version);
}

void Menu::Training() {
	gui->setGameState(CountDown);
	gui->game.startCountdown();
}

void Menu::quitGame() {
	gui->window->close();
}
