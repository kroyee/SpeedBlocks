#ifndef ALERTSUI_H
#define ALERTSUI_H

#include "guiBase.h"

class AlertsUI : public guiBase {
	struct alertData {
		tgui::Label::Ptr label;
		tgui::Button::Ptr button;
		uint16_t id;
	};
	std::list<alertData> alerts;
	tgui::Label::Ptr info, empty;
	tgui::Button::Ptr clearAll;
	uint16_t idcount=0;
public:
	AlertsUI(sf::Rect<int> _pos, Resources& _res, tgui::Panel::Ptr parent);

	void addAlert(const std::string& msg);
	void removeAlert(uint16_t _id);
	void removeAll();
	void update();
};

#endif