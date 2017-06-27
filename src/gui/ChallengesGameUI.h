#ifndef CHALLENGESGAMEUI_H
#define CHALLENGESGAMEUI_H

#include "guiBase.h"

class UI;

class ChallengesGameUI : public guiBase {
public:
	tgui::Button::Ptr startChallenge;

	tgui::Panel::Ptr racePanel;
	tgui::EditBox::Ptr raceTimeElapsed;
	tgui::EditBox::Ptr raceLinesRemaining;
	tgui::EditBox::Ptr raceBlocksUsed;


	tgui::Panel::Ptr cheesePanel;
	tgui::EditBox::Ptr cheeseTimeElapsed;
	tgui::EditBox::Ptr cheeseLinesRemaining;
	tgui::EditBox::Ptr cheeseBlocksUsed;

	void create(sf::Rect<int> _pos, UI* _ui);

	void clear();
	void update();
	sf::String displayTime(const sf::Time& time);
	void hideAllPanels();
	void showPanel(sf::Uint16 whichPanel);
};

#endif