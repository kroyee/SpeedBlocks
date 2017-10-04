#include "AlertsUI.h"
#include "optionSet.h"
#include "GameSignals.h"
#include <SFML/Network.hpp>

AlertsUI::AlertsUI(sf::Rect<int> _pos, Resources& _res, tgui::Panel::Ptr parent) : guiBase(_pos, _res, parent) {

	empty = resources.gfx->themeTG->load("Label");
	empty->setPosition(100, 50);
	empty->setText("You have no alerts at the moment.");
	panel->add(empty);

	info = resources.gfx->themeTG->load("Label");
	info->setPosition(40, 533);
	info->setText("Press on the alert to remove it or");
	info->hide();
	panel->add(info);

	clearAll = resources.gfx->themeTG->load("Button");
	clearAll->setPosition(320, 530);
	clearAll->setSize(80, 25);
	clearAll->setText("clear all");
	clearAll->connect("Pressed", &AlertsUI::removeAll, this);
	clearAll->hide();
	panel->add(clearAll);

	Signals::AddAlert.connect(&AlertsUI::addAlert, this);

	Net::takePacket(10, [&](sf::Packet &packet){
		sf::String msg;
		packet >> msg;
		addAlert(msg);
	});
}

void AlertsUI::addAlert(const sf::String& msg) {
	alertData newalert;
	newalert.id = idcount;
	idcount++;

	alerts.push_back(newalert);

	alerts.back().label = resources.gfx->themeTG->load("Label");
	alerts.back().label->setText(msg);
	alerts.back().label->setMaximumTextWidth(520);
	panel->add(alerts.back().label);

	alerts.back().button = resources.gfx->themeTG->load("Button");
	alerts.back().button->connect("Pressed", &AlertsUI::removeAlert, this, newalert.id);
	panel->add(alerts.back().button);

	update();
	Signals::PlaySound(16);
}

void AlertsUI::removeAlert(sf::Uint16 _id) {
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
		Signals::HideAlert();
		info->hide();
		clearAll->hide();
		empty->show();
		return;
	}
	Signals::ShowAlert();
	info->show();
	clearAll->show();
	empty->hide();
	int posY = 0;
	for (auto& alert : alerts) {
		int height = alert.label->getSize().y;
		alert.button->setPosition(0, posY);
		alert.button->setSize(560, height+10);
		alert.label->setPosition(20, posY+10);
		posY += height+20;
	}
}