#ifndef SLIDEMENU_H
#define SLIDEMENU_H

#include <TGUI/TGUI.hpp>
#include "GuiBase.h"

class Resources;

class SlideMenu : public GuiBase {
private:
	tgui::Picture::Ptr background;
	tgui::Tab::Ptr tab;
	sf::Time last;
	bool active, mouseOver;
public:
	float posX;
	SlideMenu(sf::Rect<int> _pos, Resources& _res);

	void handleEvent(sf::Event& event);
	void update(const sf::Time& now);
	virtual void show();
	virtual void hide();
	void tabSelect(std::string selected);
	void hideAllTabs();
	void dark();
	void light();
};

#endif
