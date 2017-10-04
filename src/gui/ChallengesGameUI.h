#ifndef CHALLENGESGAMEUI_H
#define CHALLENGESGAMEUI_H

#include "guiBase.h"
#include <array>
#include <memory>

class Resources;
struct GameplayData;
class BaseChallenge;

static const int LabelCount = 7;

class ChallengesGameUI : public guiBase {
public:
	tgui::Button::Ptr startChallenge;

	std::array<tgui::EditBox::Ptr, LabelCount> editBox;
	std::array<tgui::Label::Ptr, LabelCount> label;
	tgui::Label::Ptr header;
	tgui::Label::Ptr specLabel;
	tgui::EditBox::Ptr specEditBox;
	std::unique_ptr<BaseChallenge> challenge;

	ChallengesGameUI(sf::Rect<int> _pos, Resources& _res);

	void clear();
	sf::String displayTime(const sf::Time& time);
	void openChallenge(sf::Uint16 whichPanel);

	void hideStartChallengeButton();
};

enum class Challenges { Base, Race, Cheese, Survivor };

class BaseChallenge {
public:
	ChallengesGameUI & ref;
	int specIndex;
	Challenges type;
	BaseChallenge(ChallengesGameUI &_ref);
	void setLabel(int i, const sf::String& text);
	void setSpec(int i);
	void updateSpec();
	void setTime();
	virtual void update(GameplayData&) = 0;
	virtual void clear();
	virtual ~BaseChallenge();
};

class CH_Race : public BaseChallenge {
public:
	CH_Race(ChallengesGameUI&);
	~CH_Race() = default;
	void update(GameplayData&);
};

class CH_Cheese : public BaseChallenge {
public:
	CH_Cheese(ChallengesGameUI&);
	~CH_Cheese() = default;
	void update(GameplayData&);
};

class CH_Survivor : public BaseChallenge {
	sf::Time lineAdd;
public:
	CH_Survivor(ChallengesGameUI&);
	void update(GameplayData&);
	void clear();
};

#endif