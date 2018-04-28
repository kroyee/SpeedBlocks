#include "MainMenu.h"
#include "GameSignals.h"
#include "Resources.h"
#include "Textures.h"

Menu::Menu(sf::Rect<int> _pos, Resources& _res) : GuiBase(_pos, _res) {

	tgui::Picture::Ptr logo = tgui::Picture::create(resources.gfx->texture("logo"));
	logo->setPosition(0, -20);
	panel->add(logo);

	auto training = button1("Training");
	training->connect("pressed", &Menu::Training, this);

	auto quit = button1("Quit");
	quit->connect("pressed", &Menu::quitGame, this);

	align<Size<340, 80>>(60, 300, 30) << training << quit;

	auto version_label = label1("v"+std::to_string(resources.version_major)+"."+std::to_string(resources.version_minor)+"."+std::to_string(resources.version_patch));
	align<>(5,575) << version_label;
}

static auto& Show = Signal<void, int>::get("Show");

void Menu::Training() {
	hide();
	Show(16);
}

void Menu::quitGame() {
	resources.window.close();
}
