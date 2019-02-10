#ifndef SCROLLLISTOLD_H
#define SCROLLLISTOLD_H

#include "GuiBase.h"

class Resources;

struct ListItem {
    std::string name;
    uint16_t id;
    tgui::Button::Ptr button;
    tgui::Label::Ptr label;
};

class ScrollList_old : public GuiBase {
   public:
    std::vector<ListItem> items;
    tgui::Scrollbar::Ptr scroll;
    sf::Rect<int> pos;

    ScrollList_old(sf::Rect<int> _pos, Resources& _res);
    ScrollList_old(sf::Rect<int> _pos, Resources& _res, os::Panel& parentPanel);
    void addItem(const std::string& name, const std::string& labelStr, uint16_t id);
    void removeItem(uint16_t id);
    void removeAllItems();
    void setItemPos();
    void listScrolled(unsigned scrollpos);
    void scrolled(sf::Event& event);
};

#endif