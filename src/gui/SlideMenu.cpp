#include "SlideMenu.h"
#include "GameSignals.h"
#include "optionSet.h"
#include "Resources.h"

static auto& Show = Signal<void, int>::get("Show");
static auto& Hide = Signal<void, int>::get("Hide");
static auto& ShowOptions = Signal<void, int>::get("ShowOptions");

SlideMenu::SlideMenu(sf::Rect<int> _pos, Resources& _res) : GuiBase(_pos, _res), active(false), mouseOver(false) {

	background = tgui::Picture::create(resources.gfx->texture("menu_light"));
	background->setOpacity(0.85);
	panel->add(background);

	tab = resources.gfx->load("Tab");
	tab->setPosition(40, 0);
	tab->add("Server");
	tab->add("Game play");
	tab->add("Visual");
	tab->add("Video");
	tab->add("Sound");
	tab->add("Bugs");
	tab->select(0);
	tab->connect("TabSelected", &SlideMenu::tabSelect, this);
	panel->add(tab);

	posX = 920;
}

void SlideMenu::handleEvent(sf::Event& event) {
	if (event.type == sf::Event::KeyPressed)
		if (event.key.code == resources.options->menu) {
			active = !active;
			panel->focus();
		}
	if (event.type == sf::Event::MouseMoved && resources.options->mouseMenu) {
		sf::Vector2f pos = resources.window.mapPixelToCoords(sf::Mouse::getPosition(resources.window));
		if (pos.x > posX) {
			active = true;
			mouseOver = true;
			panel->focus();
		}
		else {
			if (mouseOver)
				active = false;
			mouseOver = false;
		}
	}
}

void SlideMenu::update(const sf::Time& now) {
	float delta = (now-last).asMicroseconds() / 1000.0;
	last=now;
	if (active)
		posX -= delta;
	else
		posX += delta;
	if (posX > 920)
		posX=920;
	else if (posX < 360)
		posX=360;

	panel->setPosition(posX, 0);
}

void SlideMenu::show() { panel->show(); }

void SlideMenu::hide() { panel->hide(); }

void SlideMenu::tabSelect(std::string selected) {
	hideAllTabs();
	if (selected == "Server")
		Show(14);
	if (selected == "Game play")
		ShowOptions(1);
	if (selected == "Visual")
		ShowOptions(0);
	if (selected == "Video")
		ShowOptions(2);
	if (selected == "Sound")
		ShowOptions(3);
	if (selected == "Bugs")
		Show(7);
}

void SlideMenu::hideAllTabs() {
	Hide(2);
	Hide(7);
	Hide(14);
}

void SlideMenu::dark() {
	panel->remove(background);
	background = tgui::Picture::create(resources.gfx->texture("menu_dark"));
	panel->add(background);
	background->moveToBack();
}

void SlideMenu::light() {
	panel->remove(background);
	background = tgui::Picture::create(resources.gfx->texture("menu_light"));
	panel->add(background);
	background->moveToBack();
}
