#ifndef CONNECTING_H
#define CONNECTING_H

#include "guiBase.h"

class Resources;

class Connecting : public guiBase {
public:
	tgui::Label::Ptr label;
	tgui::TextBox::Ptr changelog;
	tgui::Button::Ptr cancel, apply;
	Connecting(sf::Rect<int> _pos, Resources& _res);

	void setText(const sf::String& text);
};

#endif