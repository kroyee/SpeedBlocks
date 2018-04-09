#ifndef GAMESTANDINGS_H
#define GAMESTANDINGS_H

#include "guiBase.h"

namespace sf { class Packet; }

class Resources;

class GameStandings : public guiBase {
public:
	tgui::Label::Ptr rounds;
	tgui::Label::Ptr p1_sets;
	tgui::Label::Ptr p2_sets;
	tgui::Label::Ptr p1_rounds;
	tgui::Label::Ptr p2_rounds;

	uint16_t p1_id, p2_id;
	uint8_t _p1_sets, _p2_sets, _p1_rounds, _p2_rounds;

	GameStandings(sf::Rect<int> _pos, Resources& _res);

	void setResults(sf::Packet &packet);
	void setWaitTime(uint16_t time);
	void alignResult();
};

#endif