#include "MainMenu.h"
#include "GameSignals.h"
#include "Resources.h"
#include "Textures.h"

Menu::Menu(sf::Rect<int> _pos, Resources& _res) : GuiBase(_pos, _res) {
    os::Picture logo(resources.gfx->texture("logo"));
    logo.pos(0, -20).add_to(panel);

    align<os::Size<340, 80>>(60, 300, 30) << os::Button().connect("pressed", &Menu::Training, this) << os::Button().connect("pressed", &Menu::quitGame, this);

    os::Label()
        .text("v" + std::to_string(resources.version_major) + "." + std::to_string(resources.version_minor) + "." + std::to_string(resources.version_patch))
        .pos(5, 575)
        .add_to(panel);
}

static auto& Show = Signal<void, int>::get("Show");

void Menu::Training() {
    hide();
    Show(16);
}

void Menu::quitGame() { resources.window.close(); }
