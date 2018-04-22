#ifndef SCROLLLIST_H
#define SCROLLLIST_H

#include "GuiBase.h"

class Resources;

class ListItem {
public:
	std::string name;
	uint16_t id;
	tgui::Button::Ptr button;
	tgui::Label::Ptr label;
};

class ScrollList : public GuiBase {
public:
	std::list<ListItem> items;
	tgui::Scrollbar::Ptr scroll;
	sf::Rect<int> pos;

	ScrollList(sf::Rect<int> _pos, Resources& _res);
	ScrollList(sf::Rect<int> _pos, Resources& _res, tgui::Panel::Ptr parentPanel);
	void addItem(const std::string& name, const std::string& labelStr, uint16_t id);
	void removeItem(uint16_t id);
	void removeAllItems();
	void setItemPos();
	void listScrolled(int scrollpos);
	void scrolled(sf::Event& event);
};

#endif