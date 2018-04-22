#ifndef POPUPMENU_H
#define POPUPMENU_H

#include "GuiBase.h"

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

class PopupMenu;

class PopupMenuItem {
public:
	bool submenu;
	tgui::Label::Ptr label;
	std::unique_ptr<PopupMenu> menu;
	std::function<void()> function;
	Resources& resources;
	PopupMenu& parent;

	PopupMenuItem(Resources& resources, PopupMenu& parent);
	PopupMenuItem(Resources& resources, std::function<void()> f, PopupMenu& parent);
	void createLabel(bool _submenu, const std::string & text);
};

class PopupMenu : public GuiBase {
public:
	std::deque<PopupMenuItem> items;
	PopupMenu* parent;
	sf::Rect<int> stayInside;
	int width, height;
	bool openToLeft=false;
public:
	PopupMenu(Resources& res);

	PopupMenu& add(const std::string & text, std::function<void()> f);
	PopupMenu& add(const std::string & text);
	PopupMenu* get(const std::string & text);
	void childClosed();
	void hideMe();
	void hide();
	void show(int x, int y);
	void setBoundery(const sf::Rect<int>& _stayInside);
	void showIfInside(const sf::Vector2i&);

	void update();
};

#endif
