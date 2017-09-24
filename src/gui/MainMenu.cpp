#include "MainMenu.h"
#include "Signal.h"

Menu::Menu(sf::Rect<int> _pos, Resources& _res) : guiBase(_pos, _res) {

	tgui::Picture::Ptr logo = tgui::Picture::create(resources.gfx->logoTexture);
	logo->setPosition(0, -20);
	panel->add(logo);

	tgui::Button::Ptr Tr = resources.gfx->themeTG->load("Button");
	Tr->setSize(340, 80);
	Tr->setPosition(60, 300);
	Tr->setText("Training");
	Tr->setTextSize(68);
	Tr->setFont(resources.gfx->typewriter);
	Tr->connect("pressed", &Menu::Training, this);
	panel->add(Tr);

	tgui::Button::Ptr Quit = resources.gfx->themeTG->load("Button");
	Quit->setSize(190, 80);
	Quit->setPosition(135, 420);
	Quit->setText("Quit");
	Quit->setTextSize(68);
	Quit->setFont(resources.gfx->typewriter);
	Quit->connect("pressed", &Menu::quitGame, this);
	panel->add(Quit);

	tgui::Label::Ptr version = resources.gfx->themeTG->load("Label");
	version->setPosition(5,575);
	version->setText("v"+std::to_string(resources.version_major)+"."+std::to_string(resources.version_minor)+"."+std::to_string(resources.version_patch));
	panel->add(version);
}

void Menu::Training() {
	Signals::StartCountDown();
	Signals::SetGameState(GameStates::CountDown);
}

void Menu::quitGame() {
	resources.window.close();
}
