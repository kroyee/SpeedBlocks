#ifndef LOGINBOX_H
#define LOGINBOX_H

#include "guiBase.h"

class UI;

class LoginBox : public guiBase {
public:
	tgui::EditBox::Ptr LiE1;
	void create(sf::Rect<int> _pos, UI* _gui, tgui::Panel::Ptr parent);
	void login(const sf::String& name, const sf::String& pass, sf::Uint8 guest);
	void open();
	void sendLogin(const sf::String& hashorname, sf::Uint8 guest);
	void regPressed();
	void forgotPressed();
};

#endif