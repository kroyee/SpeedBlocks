#include "ScrollList.h"
#include "GameSignals.h"
#include "Resources.h"

static auto& JoinRoom = Signal<void, int>::get("JoinRoom");

ScrollList::ScrollList(sf::Rect<int> _pos, Resources& _res) : GuiBase(_pos, _res) {
	pos = _pos;

	scroll = resources.gfx->load("Scrollbar");
	scroll->setSize(30, pos.height-10);
	scroll->setPosition(pos.width-35, 5);
	scroll->setMaximum(0);
	scroll->setLowValue(1);
	scroll->connect("ValueChanged", &ScrollList::listScrolled, this);
	panel->add(scroll);
}

ScrollList::ScrollList(sf::Rect<int> _pos, Resources& _res, tgui::Panel::Ptr parentPanel) : GuiBase(_pos, _res, parentPanel) {
	pos = _pos;

	scroll = resources.gfx->load("Scrollbar");
	scroll->setSize(30, pos.height-10);
	scroll->setPosition(pos.width-35, 5);
	scroll->setMaximum(0);
	scroll->setLowValue(1);
	scroll->connect("ValueChanged", &ScrollList::listScrolled, this);
	panel->add(scroll);
}

void ScrollList::addItem(const std::string& name, const std::string& labelStr, uint16_t id) {
	ListItem newItem;
	items.push_back(newItem);
	items.back().name = name;
	items.back().button = resources.gfx->load("Button");
	items.back().button->setText(name);
	items.back().button->setSize(300, 100);
	items.back().button->connect("Pressed", [=](){ JoinRoom(id); });
	panel->add(items.back().button);

	items.back().label = resources.gfx->load("Label");
	items.back().label->setTextSize(14);
	items.back().label->setText(labelStr);
	items.back().label->disable(false);
	panel->add(items.back().label);

	items.back().id = id;

	setItemPos();
}

void ScrollList::removeItem(uint16_t id) {
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

void ScrollList::removeAllItems() {
	for (auto&& item : items) {
		panel->remove(item.button);
		panel->remove(item.label);
	}
	items.clear();
}

void ScrollList::setItemPos() {
	short height = items.size() * 120 + 20;
	if (height > pos.height) {
		height-=pos.height;
		height/=30;
		height++;
		scroll->setMaximum(height);
	}
	else
		scroll->setMaximum(0);
	listScrolled(scroll->getValue());
}

void ScrollList::listScrolled(int scrollpos) {
	for (auto it = items.begin(); it != items.end(); it++) {
		int i = std::distance(items.begin(), it);
		it->button->setPosition(50, i*120 - scrollpos*30 + 10);
		it->label->setPosition(50, i*120 - scrollpos*30 + 90);
	}
}

void ScrollList::scrolled(sf::Event& event) {
	if (panel->isVisible())
		if (event.type == sf::Event::MouseWheelScrolled)
			if (event.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel)
				if (mouseOver(panel, event.mouseWheelScroll.x, event.mouseWheelScroll.y))
					if (!mouseOver(scroll, event.mouseWheelScroll.x, event.mouseWheelScroll.y)) {
						short cur = scroll->getValue();
						cur-=event.mouseWheelScroll.delta;
						if (cur<0)
							cur=0;
						scroll->setValue(cur);
					}
}
