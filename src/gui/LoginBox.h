#ifndef LOGINBOX_H
#define LOGINBOX_H

#include "guiBase.h"
#include "PatchCheck.h"
#include "Connecting.h"
#include <thread>

class Resources;

class LoginBox : public guiBase {
private:
	bool edited=false;
	sf::String name, pass;
public:
	tgui::EditBox::Ptr LiE1;
	tgui::EditBox::Ptr LiE2;
	tgui::EditBox::Ptr LiE3;
	std::thread t;
	PatchCheck patcher;
	Connecting connectingScreen;
	LoginBox(sf::Rect<int> _pos, Resources& _res, tgui::Panel::Ptr parent);
	void launchLogin(sf::Uint8 guest);
	void login(sf::String name, sf::String pass, sf::Uint8 guest);
	virtual void show();
	void sendLogin(const sf::String& hashorname, sf::Uint8 guest);
	void regPressed();
	void forgotPressed();
	void checkStatus();
	bool isThreadJoinable();
	void tellPatcherToQuit();
};

#endif