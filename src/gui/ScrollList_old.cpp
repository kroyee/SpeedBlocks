#include "ScrollList_old.h"
#include <iostream>
#include "GameSignals.h"
#include "Resources.h"

static auto& JoinRoom = Signal<void, uint16_t>::get("JoinRoom");

ScrollList_old::ScrollList_old(sf::Rect<int> _pos, Resources& _res) : GuiBase(_pos, _res) {
    pos = _pos;

    scroll = tgui::Scrollbar::create();
    scroll->setSize(30, pos.height - 10);
    scroll->setPosition(pos.width - 35, 5);
    scroll->setMaximum(0);
    scroll->connect("ValueChanged", &ScrollList_old::listScrolled, this);
    panel->add(scroll);
}

ScrollList_old::ScrollList_old(sf::Rect<int> _pos, Resources& _res, os::Panel& parentPanel) : GuiBase(_pos, _res, parentPanel) {
    pos = _pos;

    scroll = tgui::Scrollbar::create();
    scroll->setSize(30, pos.height - 10);
    scroll->setPosition(pos.width - 35, 5);
    scroll->setMaximum(0);
    scroll->connect("ValueChanged", &ScrollList_old::listScrolled, this);
    panel->add(scroll);
}

void ScrollList_old::addItem(const std::string& name, const std::string& labelStr, uint16_t id) {
    items.push_back({});
    items.back().name = name;
    items.back().button = tgui::Button::create();
    items.back().button->setText(name);
    items.back().button->setSize(300, 100);
    items.back().button->connect("Pressed", [&, id]() { JoinRoom(id); });
    panel->add(items.back().button);

    items.back().label = tgui::Label::create();
    items.back().label->setTextSize(14);
    items.back().label->setText(labelStr);
    panel->add(items.back().label);

    items.back().id = id;

    setItemPos();
}

void ScrollList_old::removeItem(uint16_t id) {
    for (auto it = items.begin(); it != items.end(); it++) {
        if (it->id == id) {
            panel->remove(it->button);
            panel->remove(it->label);
            it = items.erase(it);
            setItemPos();
            return;
        }
    }
}

void ScrollList_old::removeAllItems() {
    for (auto&& item : items) {
        panel->remove(item.button);
        panel->remove(item.label);
    }
    items.clear();
}

void ScrollList_old::setItemPos() {
    short height = items.size() * 120 + 20;
    if (height > pos.height) {
        height -= pos.height;
        height /= 30;
        height++;
        scroll->setMaximum(height);
    } else
        scroll->setMaximum(0);
    listScrolled(scroll->getValue());
}

void ScrollList_old::listScrolled(unsigned scrollpos) {
    for (auto it = items.begin(); it != items.end(); it++) {
        int i = std::distance(items.begin(), it);
        it->button->setPosition(50, i * 120 - scrollpos * 30 + 10);
        it->label->setPosition(50, i * 120 - scrollpos * 30 + 90);
    }
}

void ScrollList_old::scrolled(sf::Event& event) {
    if (panel->isVisible())
        if (event.type == sf::Event::MouseWheelScrolled)
            if (event.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel)
                if (mouseOver(panel.get(), event.mouseWheelScroll.x, event.mouseWheelScroll.y))
                    if (!mouseOver(scroll, event.mouseWheelScroll.x, event.mouseWheelScroll.y)) {
                        short cur = scroll->getValue();
                        cur -= event.mouseWheelScroll.delta;
                        if (cur < 0) cur = 0;
                        scroll->setValue(cur);
                    }
}