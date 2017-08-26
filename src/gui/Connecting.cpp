#include "Connecting.h"
#include "gui.h"

void Connecting::create(sf::Rect<int> _pos, UI* _gui) {
	createBase(_pos, _gui);

	label = gui->themeTG->load("Label");
	label->setPosition(0, 40);
	label->setSize(300, 50);
	label->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Center);
	label->setText("Connecting to server...");
	panel->add(label);
}