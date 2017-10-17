#ifndef POPUPMENU_H
#define POPUPMENU_H

#include "guiBase.h"

class PopupMenuItem;

class PopupMenu : public guiBase {
public:
	std::deque<PopupMenuItem> items;
	PopupMenu* parent;
	sf::Rect<int> stayInside;
	int width, height;
	bool openToLeft=false;
public:
	PopupMenu(Resources& res);

	void addItem(const sf::String & text, std::function<void()> f);
	void addItem(const sf::String & text);
	PopupMenu* getSubMenu(const sf::String & text);
	void childClosed();
	void hideMe();
	void hide();
	void show(int x, int y);
	void setBoundery(sf::Rect<int> _stayInside);

	void update();
};

class PopupMenuItem {
public:
	bool submenu;
	tgui::Label::Ptr label;
	std::unique_ptr<PopupMenu> menu;
	std::function<void()> function;
	Resources& resources;

	PopupMenuItem(Resources& resources);
	PopupMenuItem(Resources& resources, std::function<void()> f);
	void createLabel(bool _submenu, const sf::String & text);
};

#endif