#ifndef BUGREPORT_H
#define BUGREPORT_H

#include "guiBase.h"
#include <thread>

class UI;

class BugReport : public guiBase {
public:
	sf::Rect<int> pos;

	tgui::TextBox::Ptr happened;
	tgui::TextBox::Ptr expected;
	tgui::TextBox::Ptr reproduce;
	tgui::TextBox::Ptr contact;

	bool join;

	std::thread t;

	void create(sf::Rect<int> _pos, UI* _gui, tgui::Panel::Ptr parent);

	void sendReport();
};

#endif