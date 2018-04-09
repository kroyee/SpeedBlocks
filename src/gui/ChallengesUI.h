#ifndef CHALLENGESUI_H
#define CHALLENGESUI_H

#include "guiBase.h"
#include "ScrollList.h"

namespace sf { class Packet; }

class Resources;

struct ChallengesRow {
	tgui::Label::Ptr label[4];
	tgui::Button::Ptr button;
};

class ChallengesUI : public guiBase {
public:
	ScrollList challengeList;
	tgui::Panel::Ptr leaderPanel;
	tgui::Panel::Ptr scrollPanel;
	tgui::Label::Ptr title;
	tgui::Scrollbar::Ptr scroll;
	tgui::Button::Ptr playChallenge;

	std::list<ChallengesRow> rows;

	uint16_t selectedId;
	uint16_t itemsInScrollPanel;
	uint8_t columns;
	uint16_t width[4];

	ChallengesUI(sf::Rect<int> _pos, Resources& _res, tgui::Panel::Ptr parentPanel);
	void makeList(sf::Packet &packet);
	void makeLeaderboard(sf::Packet &packet);
	void play();
	virtual void show();

	void viewReplay(uint16_t slot);

	void listScrolled(int scrollpos);
	void scrolled(sf::Event& event);
};

#endif