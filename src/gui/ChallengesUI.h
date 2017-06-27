#ifndef CHALLENGESUI_H
#define CHALLENGESUI_H

#include "guiBase.h"
#include "ScrollList.h"

class UI;

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

	sf::Uint16 selectedId;
	sf::Uint16 itemsInScrollPanel;
	sf::Uint8 columns;
	sf::Uint16 width[4];

	void create(sf::Rect<int> _pos, UI* _ui, tgui::Panel::Ptr parentPanel);
	void makeList();
	void makeLeaderboard();
	void play();
	void show();

	void viewReplay(sf::Uint16 slot);

	void listScrolled(int scrollpos);
	void scrolled(sf::Event& event);
};

#endif