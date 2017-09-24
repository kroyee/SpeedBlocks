#ifndef ALERTSUI_H
#define ALERTSUI_H

#include "guiBase.h"

class AlertsUI : public guiBase {
	struct alertData {
		tgui::Label::Ptr label;
		tgui::Button::Ptr button;
		sf::Uint16 id;
	};
	std::list<alertData> alerts;
	tgui::Label::Ptr info, empty;
	tgui::Button::Ptr clearAll;
	sf::Uint16 idcount=0;
public:
	AlertsUI(sf::Rect<int> _pos, Resources& _res, tgui::Panel::Ptr parent);

	void addAlert(const sf::String& msg);
	void removeAlert(sf::Uint16 _id);
	void removeAll();
	void update();
};

#endif