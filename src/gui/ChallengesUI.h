#ifndef CHALLENGESUI_H
#define CHALLENGESUI_H

#include "guiBase.h"
#include "ScrollList.h"

class UI;

class ChallengesUI : public guiBase {
public:
	ScrollList challengeList;
	tgui::Panel::Ptr leaderPanel;
	tgui::Panel::Ptr scrollPanel;
	tgui::Label::Ptr title;
	tgui::Scrollbar::Ptr scroll;
	tgui::Button::Ptr playChallenge;

	sf::Uint16 selectedId;

	void create(sf::Rect<int> _pos, UI* _ui, tgui::Panel::Ptr parentPanel);
	void makeList();
	void makeLeaderboard();
	void play();
	void show();

	void viewReplay(sf::Uint16 slot);
};

#endif