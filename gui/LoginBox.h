#ifndef LOGINBOX_H
#define LOGINBOX_H

#include "guiBase.h"

class UI;

class LoginBox : public guiBase {
public:
	tgui::EditBox::Ptr LiE1;
	void create(sf::Rect<int> _pos, UI* _ui);
	void login(const sf::String& name, const sf::String& pass, sf::Uint8 guest);
	void closeLogin();
	void open();
};

#endif