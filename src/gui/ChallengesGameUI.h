#ifndef CHALLENGESGAMEUI_H
#define CHALLENGESGAMEUI_H

#include <array>
#include <memory>
#include "GuiBase.h"

class Resources;
struct GameplayData;
class BaseChallenge;

static const int LabelCount = 7;

class ChallengesGameUI : public GuiBase {
   public:
    os::Button startChallenge;

    std::array<os::Edit, LabelCount> info_fields;
    os::Label header;
    os::Edit special;
    std::unique_ptr<BaseChallenge> challenge;

    ChallengesGameUI(sf::Rect<int> _pos, Resources& _res);

    void clear();
    std::string displayTime(const sf::Time& time);
    void openChallenge(uint16_t whichPanel);

    void hideStartChallengeButton();
    void hide();
};

enum class Challenges { Base, Race, Cheese, Survivor, Cheese30L };

class BaseChallenge {
   public:
    ChallengesGameUI& ref;
    int specIndex;
    Challenges type;
    BaseChallenge(ChallengesGameUI& _ref);
    void setLabel(int i, const std::string& text);
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
    ~CH_Survivor();
    void update(GameplayData&);
    void clear();
};

class CH_Cheese30L : public BaseChallenge {
    int linesAdded;

   public:
    CH_Cheese30L(ChallengesGameUI&);
    ~CH_Cheese30L();
    void update(GameplayData&);
    void clear();
};

#endif