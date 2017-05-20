#include "Connecting.h"
#include "gui.h"

void Connecting::create(sf::Rect<int> _pos, UI* _gui) {
	createBase(_pos, _gui);

	tgui::Label::Ptr WcL = gui->themeTG->load("Label");
	WcL->setPosition(0, 40);
	WcL->setSize(300, 50);
	WcL->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Center);
	WcL->setText("Connecting to server...");
	panel->add(WcL);
}