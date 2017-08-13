#include "AlertsUI.h"
#include "gui.h"
#include "SlideMenu.h"

void AlertsUI::create(sf::Rect<int> _pos, UI* _gui, tgui::Panel::Ptr parent) {
	createBase(_pos, _gui, parent);

	empty = gui->themeTG->load("Label");
	empty->setPosition(100, 50);
	empty->setText("You have no alerts at the moment.");
	panel->add(empty);

	info = gui->themeTG->load("Label");
	info->setPosition(40, 533);
	info->setText("Press on the alert to remove it or");
	info->hide();
	panel->add(info);

	clearAll = gui->themeTG->load("Button");
	clearAll->setPosition(320, 530);
	clearAll->setSize(80, 25);
	clearAll->setText("clear all");
	clearAll->connect("Pressed", &AlertsUI::removeAll, this);
	clearAll->hide();
	panel->add(clearAll);
}

void AlertsUI::addAlert(const sf::String& msg) {
	alertData newalert;
	newalert.id = idcount;
	idcount++;

	alerts.push_back(newalert);

	alerts.back().label = gui->themeTG->load("Label");
	alerts.back().label->setText(msg);
	alerts.back().label->setMaximumTextWidth(520);
	panel->add(alerts.back().label);

	alerts.back().button = gui->themeTG->load("Button");
	alerts.back().button->connect("Pressed", &AlertsUI::removeAlert, this, newalert.id);
	panel->add(alerts.back().button);

	update();

	if (gui->options.sound)
		gui->resources.sounds.alert();
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
		gui->slideMenu->hideAlert();
		info->hide();
		clearAll->hide();
		empty->show();
		return;
	}
	gui->slideMenu->showAlert();
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