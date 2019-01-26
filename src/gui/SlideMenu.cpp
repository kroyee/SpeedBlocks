#include "SlideMenu.h"
#include "GameSignals.h"
#include "Options.h"
#include "Resources.h"

static auto& Show = Signal<void, int>::get("Show");
static auto& Hide = Signal<void, int>::get("Hide");
static auto& ShowOptions = Signal<void, int>::get("ShowOptions");

#include <iostream>

SlideMenu::SlideMenu(sf::Rect<int> _pos, Resources& _res) : GuiBase(_pos, _res), active(false), mouseOver(false) {
    panel.background();

    tab.pos(40, 0)
        .add("Server")
        .add("Game play")
        .add("Visual")
        .add("Video")
        .add("Sound")
        .add("Bugs")
        .select(0)
        .connect("TabSelected", &SlideMenu::tabSelect, this)
        .add_to(panel);

    posX = 920;
}

void SlideMenu::handleEvent(sf::Event& event) {
    if (event.type == sf::Event::KeyPressed)
        if (event.key.code == Options::get<sf::Keyboard::Key>("menu")) {
            active = !active;
            panel.focus();
        }
    if (event.type == sf::Event::MouseMoved && Options::get<bool>("mouseMenu")) {
        sf::Vector2f pos = resources.window.mapPixelToCoords(sf::Mouse::getPosition(resources.window));
        if (pos.x > posX) {
            active = true;
            mouseOver = true;
            panel.focus();
        } else {
            if (mouseOver) active = false;
            mouseOver = false;
        }
    }
}

void SlideMenu::update(const sf::Time& now) {
    float delta = (now - last).asMicroseconds() / 1000.0;
    last = now;
    if (active)
        posX -= delta;
    else
        posX += delta;
    if (posX > 920)
        posX = 920;
    else if (posX < 360)
        posX = 360;

    panel.pos(posX, 0);
}

void SlideMenu::show() { panel.show(); }

void SlideMenu::hide() { panel.hide(); }

void SlideMenu::tabSelect(std::string selected) {
    hideAllTabs();
    if (selected == "Server") Show(14);
    if (selected == "Game play") ShowOptions(1);
    if (selected == "Visual") ShowOptions(0);
    if (selected == "Video") ShowOptions(2);
    if (selected == "Sound") ShowOptions(3);
    if (selected == "Bugs") Show(7);
}

void SlideMenu::hideAllTabs() {
    Hide(2);
    Hide(7);
    Hide(14);
}

void SlideMenu::dark() {}

void SlideMenu::light() {}
