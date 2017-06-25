#ifndef GAMESTANDINGS_H
#define GAMESTANDINGS_H

#include "guiBase.h"

class UI;

class GameStandings : public guiBase {
public:
	tgui::Label::Ptr rounds;
	tgui::Label::Ptr p1_sets;
	tgui::Label::Ptr p2_sets;
	tgui::Label::Ptr p1_rounds;
	tgui::Label::Ptr p2_rounds;

	sf::Uint16 p1_id, p2_id;
	sf::Uint8 _p1_sets, _p2_sets, _p1_rounds, _p2_rounds;

	void create(sf::Rect<int> _pos, UI* _ui);

	void setResults();
	void setWaitTime(sf::Uint16 time);
	void alignResult();
};

#endif