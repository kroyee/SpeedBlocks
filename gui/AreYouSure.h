#ifndef AREYOUSURE_H
#define AREYOUSURE_H

#include "guiBase.h"

class UI;

class AreYouSure : public guiBase {
public:
	tgui::Label::Ptr label;
	
	void create(sf::Rect<int> _pos, UI* _ui);
	void ausY();
	void ausN();
};

#endif