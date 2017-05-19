#ifndef BUGREPORT_H
#define BUGREPORT_H

#include "guiBase.h"

class UI;

class BugReport : public guiBase {
public:
	sf::Rect<int> pos;
	tgui::ChildWindow::Ptr ChildWindow;

	tgui::TextBox::Ptr happened;
	tgui::TextBox::Ptr expected;
	tgui::TextBox::Ptr reproduce;
	tgui::TextBox::Ptr contact;

	void create(sf::Rect<int> _pos, UI* _ui);

	void bugReport();
	void minimize();
	void close();
	void sendReport();
};

#endif