#ifndef GUIBASE_H
#define GUIBASE_H

#include <TGUI/TGUI.hpp>
#include "textures.h"
#include "Resources.h"

class Resources;

struct PosAndSize {
	PosAndSize() : x(0), y(0), w(960), h(600) {}
	PosAndSize(int _x, int _y, int _w, int _h) : x(_x), y(_y), w(_w), h(_h) {}
	PosAndSize(int _x, int _y) : x(_x), y(_y), w(0), h(0) {}
	PosAndSize(sf::Rect<int> rect) : x(rect.left), y(rect.top), w(rect.width), h(rect.height) {}
	PosAndSize(sf::Rect<float> rect) : x(rect.left), y(rect.top), w(rect.width), h(rect.height) {}
	int x, y, w, h;
};

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
	bool isVisible() { return panel->isVisible(); }

	bool mouseOver(tgui::Widget::Ptr widget, int x, int y);
	bool mouseOver(tgui::Widget::Ptr widget);

	tgui::WidgetConverter loadWidget(const sf::String& widget, const PosAndSize& pos = PosAndSize(), const sf::String& text="");
	tgui::WidgetConverter loadWidgetTo(tgui::Panel::Ptr panel, const sf::String& widget, const PosAndSize& pos = PosAndSize(), const sf::String& text="");
	tgui::Panel::Ptr loadPanel(const PosAndSize& pos = PosAndSize(), bool hide=true);
	tgui::Panel::Ptr loadPanelTo(tgui::Panel::Ptr toPanel, const PosAndSize& pos = PosAndSize(), bool hide=true);
};



#endif