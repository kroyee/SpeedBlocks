#ifndef GUIBASE_H
#define GUIBASE_H

#include <TGUI/TGUI.hpp>
#include "textures.h"
#include "Resources.h"

class Resources;

class guiBase {
public:
	tgui::Panel::Ptr panel;
	Resources& resources;

	guiBase(sf::Rect<int> _pos, Resources& _res);
	guiBase(sf::Rect<int> _pos, Resources& _res, tgui::Panel::Ptr parentPanel);
	virtual ~guiBase();
	virtual void show() { panel->show(); }
	virtual void hide() { panel->hide(); }
	void enable() { panel->enable(); }
	void disable() { panel->disable(); }
	bool isVisible() { return (panel->isVisible()) ? true : false; }

	bool mouseOver(tgui::Widget::Ptr widget, int x, int y);
	bool mouseOver(tgui::Widget::Ptr widget);
};

#endif