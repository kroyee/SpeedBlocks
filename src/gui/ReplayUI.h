#ifndef REPLAYUI_H
#define REPLAYUI_H

#include "GuiBase.h"

class Resources;

class ReplayUI : public GuiBase {
   public:
    os::Label timeTotal;
    os::Button playPause;
    os::Button gameForward;
    os::Button setForward;
    os::Button gameBack;
    os::Button setBack;
    os::Label timePlayed;
    os::ProgressBar seekbar;
    os::Label rounds;
    os::Label sets;

    sf::Time pauseTime;

    std::string nameBackup;
    bool backup;

    ReplayUI(sf::Rect<int> _pos, Resources& _res);
    void show(bool showTournamentControls = false);
    void hide();

    void update(sf::Time);
    void seek(sf::Vector2f mouse);
    std::string displayTime(uint16_t timeVal);
    void pause();
};

#endif