#include "guiBase.h"
#include "gui.h"

void guiBase::createBase(sf::Rect<int> _pos, UI* _ui) {
	ui = _ui;
	panel = tgui::Panel::create();
	panel->setPosition(_pos.left, _pos.top);
	panel->setSize(_pos.width, _pos.height);
	panel->setBackgroundColor(sf::Color(255,255,255,0));
	panel->hide();
	ui->gui.add(panel);
}

void guiBase::createBase(sf::Rect<int> _pos, UI* _ui, tgui::Panel::Ptr parentPanel) {
	ui = _ui;
	panel = tgui::Panel::create();
	panel->setPosition(_pos.left, _pos.top);
	panel->setSize(_pos.width, _pos.height);
	panel->setBackgroundColor(sf::Color(255,255,255,0));
	panel->hide();
	parentPanel->add(panel);
}

bool guiBase::mouseOver(tgui::Widget::Ptr widget, int x, int y) {
	sf::Vector2f pos = widget->getAbsolutePosition();
	sf::Vector2f size = widget->getSize();
	//sf::Vector2f pos = window->mapPixelToCoords(sf::Mouse::getPosition(*window));
	if (x>=pos.x && x<=pos.x+size.x)
		if (y>=pos.y && y<=pos.y+size.y)
			return true;
	return false;
}