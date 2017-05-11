#ifndef SCROLLLIST_H
#define SCROLLLIST_H

#include <TGUI/TGUI.hpp>

class UI;

class ListItem {
public:
	sf::String name;
	sf::Uint16 id;
	tgui::Button::Ptr button;
	tgui::Label::Ptr label;
};

class ScrollList {
public:
	std::list<ListItem> items;
	tgui::Scrollbar::Ptr scroll;
	tgui::Panel::Ptr panel;
	sf::Rect<int> pos;
	UI* ui;

	void create(tgui::Panel::Ptr parentPanel, sf::Rect<int> _pos, UI* _ui);
	void addItem(const sf::String& name, const sf::String& labelStr, sf::Uint16 id);
	void removeItem(sf::Uint16 id);
	void removeAllItems();
	void setItemPos();
	void listScrolled(int scrollpos);
	void scrolled(sf::Event& event);
};

#endif