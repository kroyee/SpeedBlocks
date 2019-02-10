#ifndef CHALLENGESUI_H
#define CHALLENGESUI_H

#include "GuiBase.h"
#include "ScrollList.hpp"
#include "ScrollList_old.h"

namespace sf {
class Packet;
}

class Resources;

class ChallengesUI : public GuiBase {
   public:
    ScrollList_old challengeList;
    os::Panel leaderPanel;
    os::ScrollList scrollPanel;
    os::Label title;
    os::Button playChallenge;

    uint16_t selectedId;

    ChallengesUI(sf::Rect<int> _pos, Resources& _res, os::Panel& parentPanel);
    void play();
    virtual void show();

    void viewReplay(uint16_t slot);
};

#endif