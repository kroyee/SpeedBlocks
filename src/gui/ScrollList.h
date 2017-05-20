#ifndef SCROLLLIST_H
#define SCROLLLIST_H

#include "guiBase.h"

class UI;

class ListItem {
public:
	sf::String name;
	sf::Uint16 id;
	tgui::Button::Ptr button;
	tgui::Label::Ptr label;
};

class ScrollList : public guiBase {
public:
	std::list<ListItem> items;
	tgui::Scrollbar::Ptr scroll;
	sf::Rect<int> pos;

	void create(sf::Rect<int> _pos, UI* _gui);
	void create(sf::Rect<int> _pos, UI* _gui, tgui::Panel::Ptr parentPanel);
	void addItem(const sf::String& name, const sf::String& labelStr, sf::Uint16 id);
	void removeItem(sf::Uint16 id);
	void removeAllItems();
	void setItemPos();
	void listScrolled(int scrollpos);
	void scrolled(sf::Event& event);
};

#endif