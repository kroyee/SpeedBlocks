#include "SlideMenu.h"
#include "gui.h"
#include "Resources.h"
#include "GameOptions.h"

SlideMenu::SlideMenu(sf::Rect<int> _pos, UI* gui) : active(false), mouseOver(false) {
	createBase(_pos, gui);

	background = tgui::Picture::create(gui->resources.gfx.menuBackground_light);
	background->setOpacity(0.85);
	panel->add(background);
	
	alert = tgui::Picture::create(gui->resources.gfx.alert);
	alert->setPosition(0, 60);
	alert->hide();
	panel->add(alert);

	tab = gui->themeTG->load("Tab");
	tab->setPosition(40, 0);
	tab->add("Alerts");
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
		if (event.key.code == sf::Keyboard::LControl) {
			active = !active;
			panel->focus();
		}
	if (event.type == sf::Event::MouseMoved) {
		sf::Vector2f pos = gui->window->mapPixelToCoords(sf::Mouse::getPosition(*gui->window));
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

void SlideMenu::showAlert() { alert->show(); }

void SlideMenu::hideAlert() { alert->hide(); }

void SlideMenu::show() { panel->show(); }

void SlideMenu::hide() { panel->hide(); }

void SlideMenu::tabSelect(std::string selected) {
	hideAllTabs();
	if (selected == "Alerts")
		((guiBase*)gui->alertsUI)->show();
	if (selected == "Server")
		((guiBase*)gui->serverUI)->show();
	if (selected == "Game play")
		gui->gameOptions->show(1);
	if (selected == "Visual")
		gui->gameOptions->show(0);
	if (selected == "Video")
		gui->gameOptions->show(2);
	if (selected == "Sound")
		gui->gameOptions->show(3);
	if (selected == "Bugs")
		((guiBase*)gui->bugReport)->show();
}

void SlideMenu::hideAllTabs() {
	gui->gameOptions->hide();
	((guiBase*)gui->bugReport)->hide();
	((guiBase*)gui->alertsUI)->hide();
	((guiBase*)gui->serverUI)->hide();
}

void SlideMenu::dark() {
	panel->remove(background);
	background = tgui::Picture::create(gui->resources.gfx.menuBackground_dark);
	panel->add(background);
	background->moveToBack();
}

void SlideMenu::light() {
	panel->remove(background);
	background = tgui::Picture::create(gui->resources.gfx.menuBackground_light);
	panel->add(background);
	background->moveToBack();
}