#ifndef LOGINBOX_H
#define LOGINBOX_H

#include "guiBase.h"
#include "PatchCheck.h"
#include <thread>

class UI;

class LoginBox : public guiBase {
private:
	bool edited=false;
	sf::String name, pass;
public:
	tgui::EditBox::Ptr LiE1;
	tgui::EditBox::Ptr LiE2;
	std::thread t;
	PatchCheck patcher;
	void create(sf::Rect<int> _pos, UI* _gui, tgui::Panel::Ptr parent);
	void login(/*const sf::String& name, const sf::String& pass,*/ sf::Uint8 guest);
	void show();
	void sendLogin(const sf::String& hashorname, sf::Uint8 guest);
	void regPressed();
	void forgotPressed();
	void checkStatus();
};

#endif