#ifndef ALERTSUI_H
#define ALERTSUI_H

#include "GuiBase.h"

class AlertsUI : public GuiBase {
	struct alertData {
		tgui::Label::Ptr label;
		tgui::Button::Ptr button;
		uint16_t id;
	};
	std::list<alertData> alerts;
	uint16_t idcount=0;
public:
	AlertsUI(sf::Rect<int> _pos, Resources& _res);

	void addAlert(const std::string& msg);
	void removeAlert(uint16_t _id);
	void removeAll();
	void update();
};

#endif
