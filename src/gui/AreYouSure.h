#ifndef AREYOUSURE_H
#define AREYOUSURE_H

#include "GuiBase.h"

class Resources;

class AreYouSure : public GuiBase {
public:
	tgui::Label::Ptr label;
	
	AreYouSure(sf::Rect<int> _pos, Resources& _res);
	void ausY();
	void ausN();

	void setAUS(const std::string& text);
};

#endif