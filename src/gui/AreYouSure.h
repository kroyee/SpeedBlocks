#ifndef AREYOUSURE_H
#define AREYOUSURE_H

#include "guiBase.h"

class Resources;

class AreYouSure : public guiBase {
public:
	tgui::Label::Ptr label;
	
	AreYouSure(sf::Rect<int> _pos, Resources& _res);
	void ausY();
	void ausN();

	void setAUS(const sf::String& text);
};

#endif