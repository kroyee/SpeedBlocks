#include "guiBase.h"

guiBase::guiBase(sf::Rect<int> _pos, Resources& _res) : resources(_res) {
	panel = tgui::Panel::create();
	panel->setPosition(_pos.left, _pos.top);
	panel->setSize(_pos.width, _pos.height);
	panel->setBackgroundColor(sf::Color(255,255,255,0));
	panel->hide();
	resources.gfx->tGui.add(panel);
}

guiBase::guiBase(sf::Rect<int> _pos, Resources& _res, tgui::Panel::Ptr parentPanel) : resources(_res) {
	panel = tgui::Panel::create();
	panel->setPosition(_pos.left, _pos.top);
	panel->setSize(_pos.width, _pos.height);
	panel->setBackgroundColor(sf::Color(255,255,255,0));
	panel->hide();
	parentPanel->add(panel);
}

guiBase::~guiBase() {}

bool guiBase::mouseOver(tgui::Widget::Ptr widget, int x, int y) {
	sf::Vector2f pos = widget->getAbsolutePosition();
	sf::Vector2f size = widget->getSize();
	if (x>=pos.x && x<=pos.x+size.x)
		if (y>=pos.y && y<=pos.y+size.y)
			return true;
	return false;
}