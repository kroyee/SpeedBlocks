#include "Connecting.h"
#include "gui.h"

void Connecting::create(sf::Rect<int> _pos, UI* _ui) {
	createBase(_pos, _ui);

	tgui::Label::Ptr WcL = ui->themeTG->load("Label");
	WcL->setPosition(0, 40);
	WcL->setSize(300, 50);
	WcL->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Center);
	WcL->setText("Connecting to server...");
	panel->add(WcL);
}