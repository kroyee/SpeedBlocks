#include "AlertsUI.h"
#include "Options.h"
#include "GameSignals.h"
#include "Resources.h"
#include <SFML/Network.hpp>

static auto& PlaySound = Signal<void, int>::get("PlaySound");

AlertsUI::AlertsUI(sf::Rect<int> _pos, Resources& _res) : GuiBase(_pos, _res) {
	connectSignal("AddAlert", &AlertsUI::addAlert, this);

	Net::takePacket(10, [&](sf::Packet &packet){
		std::string msg;
		packet >> msg;
		addAlert(msg);
	});
}

void AlertsUI::addAlert(const std::string& msg) {
	alertData newalert;
	newalert.id = idcount;
	idcount++;

	alerts.push_back(newalert);

	alerts.back().label = label1(msg);
	alerts.back().label->setMaximumTextWidth(520);
	panel->add(alerts.back().label);

	alerts.back().button = button3("");
	alerts.back().button->connect("Pressed", &AlertsUI::removeAlert, this, newalert.id);
	panel->add(alerts.back().button);

	update();
	PlaySound(16);
}

void AlertsUI::removeAlert(uint16_t _id) {
	for (auto it = alerts.begin(); it != alerts.end(); it++)
		if (it->id == _id) {
			panel->remove(it->button);
			panel->remove(it->label);
			alerts.erase(it);
			update();
			return;
		}
}

void AlertsUI::removeAll() {
	for (auto& alert : alerts) {
		panel->remove(alert.button);
		panel->remove(alert.label);
	}
	alerts.clear();
	update();
}

void AlertsUI::update() {
	if (alerts.empty()) {
		hide();
		return;
	}
	show();
	unsigned posY = 0;
	for (auto& alert : alerts) {
		int height = alert.label->getSize().y;
		alert.button->setPosition(0, posY);
		alert.button->setSize(560, height+20);
		alert.label->setPosition(20, posY+10);
		posY += height+30;
	}
	panel->setSize(panel->getSize().x, posY-10);
}
