#ifndef POPUPMENU_H
#define POPUPMENU_H

#include "guiBase.h"

/*	Example usage

	PopupMenu pop(resources);
    pop.setBoundery({100, 100, 300, 300});
    pop.addItem("Heya");
    pop.addItem("Coolio");
    pop.addItem("Press to do shit", [&](){ gui.guiElements->animatedBackground.disable(); });
    pop.addItem("What's up doc?");
    auto sub = pop.getSubMenu("Heya");
    sub->addItem("Handicap");
    sub = sub->getSubMenu("Handicap");
    sub->addItem("10%");
    sub->addItem("20%");
    sub->addItem("30%");
    sub->addItem("40%");
    sub->addItem("50%");

    pop.update();  //When we update the internal panels and labels are resized ect.
*/

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