#ifndef BUGREPORT_H
#define BUGREPORT_H

#include "GuiBase.h"
#include <thread>

class Resources;

class BugReport : public GuiBase {
public:
	sf::Rect<int> pos;

	tgui::TextBox::Ptr happened;
	tgui::TextBox::Ptr expected;
	tgui::TextBox::Ptr reproduce;
	tgui::TextBox::Ptr contact;

	bool join;

	std::thread t;

	BugReport(sf::Rect<int> _pos, Resources& _res, tgui::Panel::Ptr parent);

	void sendReport();
};

#endif