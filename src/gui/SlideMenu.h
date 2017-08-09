#ifndef SLIDEMENU_H
#define SLIDEMENU_H

#include <TGUI/TGUI.hpp>
#include "guiBase.h"

class UI;

class SlideMenu : public guiBase {
private:
	tgui::Picture::Ptr background;
	tgui::Tab::Ptr tab;
	tgui::Picture::Ptr alert;
	sf::Time last;
	bool active, mouseOver;
public:
	float posX;
	SlideMenu(sf::Rect<int> _pos, UI* gui);

	void handleEvent(sf::Event& event);
	void update(const sf::Time& now);
	void showAlert();
	void hideAlert();
	void show();
	void hide();
	void tabSelect(std::string selected);
	void hideAllTabs();
	void dark();
	void light();
};

#endif