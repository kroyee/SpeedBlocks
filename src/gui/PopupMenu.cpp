#include "PopupMenu.h"
#include "Options.h"
#include "Resources.h"

void PopupMenuItem::createLabel(bool _submenu, const std::string & text) {
	submenu = _submenu;
	label = resources.gfx->load("Label");
	label->connect("MouseEntered", [&](){
		if (Options::get<uint8_t>("theme") == 2)
			label->getRenderer()->setBackgroundColor(sf::Color(148,148,148,220));
		else
			label->getRenderer()->setBackgroundColor(sf::Color(108,108,108,220));
	});
	label->connect("MouseLeft", [&](){ label->getRenderer()->setBackgroundColor(sf::Color(255,255,255,0)); });
	if (!submenu) {
		label->connect("MousePressed", [&](){ function(); parent.hide(); });
		label->setText(text);
	}
	else {
		menu = std::unique_ptr<PopupMenu>(new PopupMenu(resources));
		label->connect("MouseEntered", [&](){ menu->show(label->getAbsolutePosition().x + label->getSize().x, label->getAbsolutePosition().y); });
		label->connect("MouseLeft", [&](){ if (!menu->mouseOver(menu->panel)) menu->hide(); });
		label->setText(text + " >");
	}
}

PopupMenuItem::PopupMenuItem(Resources& _resources, PopupMenu& parent_) : resources(_resources), parent(parent_) {}

PopupMenuItem::PopupMenuItem(Resources& _resources, std::function<void()> f, PopupMenu& parent_) : function(std::move(f)), resources(_resources), parent(parent_) {}

PopupMenu::PopupMenu(Resources& res) : GuiBase({0,0,0,0}, res), parent(nullptr) {
	panel->connect("MouseLeft", &PopupMenu::hideMe, this);
	panel->setBackgroundColor(sf::Color(128,128,128,220));
	stayInside = {0,0,static_cast<int>(resources.window.getSize().x), static_cast<int>(resources.window.getSize().y)};
}

PopupMenu& PopupMenu::add(const std::string & text, std::function<void()> f) {
	items.emplace_back(PopupMenuItem(resources, f, *this));
	items.back().createLabel(false, text);
	panel->add(items.back().label);
	return *this;
}

PopupMenu& PopupMenu::add(const std::string & text) {
	items.emplace_back(PopupMenuItem(resources, *this));
	items.back().createLabel(true, text);
	items.back().menu->parent = this;
	items.back().menu->stayInside = stayInside;
	panel->add(items.back().label);
	return *items.back().menu;
}

PopupMenu* PopupMenu::get(const std::string & text) {
	for (auto& item : items)
		if (item.submenu && item.label->getText() == text + " >")
			return item.menu.get();
	return nullptr;
}

void PopupMenu::update() {
	width=0;
	for (auto& item : items) {
		if (item.label->getSize().x > width) {
			width = item.label->getSize().x;
			if (item.submenu)
				item.menu->update();
		}
	}
	height = 0;
	for (auto& item : items) {
		item.label->setSize(width, item.label->getSize().y);
		item.label->setPosition(0, height);
		height += item.label->getSize().y;
	}
	panel->setSize(width, height);
}

void PopupMenu::childClosed() {
	if (mouseOver(panel))
		return;
	hide();
}

void PopupMenu::hideMe() {
	for (auto& item : items)
		if (item.submenu)
			if (mouseOver(item.menu->panel))
				return;
	hide();
}

void PopupMenu::hide() {
	panel->hide();
	if (parent != nullptr)
		parent->childClosed();
}

void PopupMenu::show(int x, int y) {
	if (parent) {
		if (parent->openToLeft) {
			openToLeft=true;
			x = x - width - parent->width;
		}
		else if (x+width > stayInside.left + stayInside.width) {
			openToLeft=true;
			x = x - width - parent->width;
		}
	}
	else {
		sf::Vector2f coords = resources.window.mapPixelToCoords({x-5, y-5});
		x=coords.x; y=coords.y;
		if (x<stayInside.left)
			x=stayInside.left;
		if (x + width > stayInside.left + stayInside.width)
			x=stayInside.left+stayInside.width - width;
	}
	if (y<stayInside.top)
		y=stayInside.top;
	if (y + height > stayInside.top + stayInside.height)
		y=stayInside.top + stayInside.height - height;

	panel->setPosition(x, y);
	panel->show();
}

void PopupMenu::setBoundery(const sf::Rect<int>& _stayInside) {
	stayInside = _stayInside;
	for (auto& item : items)
		if (item.submenu)
			item.menu->setBoundery(stayInside);
}

void PopupMenu::showIfInside(const sf::Vector2i& pos) {
	if (stayInside.contains(pos))
		show(pos.x, pos.y);
}
