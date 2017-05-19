#ifndef GUIBASE_H
#define GUIBASE_H

#include <TGUI/TGUI.hpp>

class UI;

class guiBase {
public:
	tgui::Panel::Ptr panel;
	UI* ui;

	void createBase(sf::Rect<int> _pos, UI* _ui);
	void createBase(sf::Rect<int> _pos, UI* _ui, tgui::Panel::Ptr parentPanel);
	void show() { panel->show(); }
	void hide() { panel->hide(); }
	void enable() { panel->enable(); }
	void disable() { panel->disable(); }
	bool isVisible() { return (panel->isVisible()) ? true : false; }

	bool mouseOver(tgui::Widget::Ptr widget, int x, int y);
};

#endif