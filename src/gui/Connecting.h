#ifndef CONNECTING_H
#define CONNECTING_H

#include "guiBase.h"

class UI;

class Connecting : public guiBase {
public:
	tgui::Label::Ptr label;
	void create(sf::Rect<int> _pos, UI* _gui);
};

#endif