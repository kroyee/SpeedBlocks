#ifndef CHALLENGESUI_H
#define CHALLENGESUI_H

#include "GuiBase.h"
#include "ScrollList.h"
#include "ScrollList.hpp"

namespace sf {
class Packet;
}

class Resources;

class ChallengesUI : public GuiBase {
   public:
    ScrollList challengeList;
    os::Panel leaderPanel;
    os::ScrollList scrollPanel;
    os::Label title;
    os::Button playChallenge;

    uint16_t selectedId;

    ChallengesUI(sf::Rect<int> _pos, Resources& _res, tgui::Panel::Ptr parentPanel);
    void makeList(sf::Packet& packet);
    void makeLeaderboard(sf::Packet& packet);
    void play();
    virtual void show();

    void viewReplay(uint16_t slot);

    void listScrolled(int scrollpos);
    void scrolled(sf::Event& event);
};

#endif