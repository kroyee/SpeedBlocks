#ifndef SLIDEMENU_H
#define SLIDEMENU_H

#include <TGUI/TGUI.hpp>
#include "GuiBase.h"

class Resources;

class SlideMenu : public GuiBase {
private:
	tgui::Picture::Ptr background;
	tgui::Tab::Ptr tab;
	tgui::Picture::Ptr alert;
	sf::Time last;
	bool active, mouseOver;
public:
	float posX;
	SlideMenu(sf::Rect<int> _pos, Resources& _res);

	void handleEvent(sf::Event& event);
	void update(const sf::Time& now);
	void showAlert();
	void hideAlert();
	virtual void show();
	virtual void hide();
	void tabSelect(std::string selected);
	void hideAllTabs();
	void dark();
	void light();
};

#endif