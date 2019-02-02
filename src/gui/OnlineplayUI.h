#ifndef ONLINEPLAYUI_H
#define ONLINEPLAYUI_H

#include "ChallengesUI.h"
#include "GuiBase.h"
#include "ScrollList.h"
#include "TournamentUI.h"

class Resources;

class OnlineplayUI : public GuiBase {
   public:
    os::Tabs opTab;
    os::Panel CreateRoom;

    ScrollList roomList;
    ScrollList tournamentList;

    TournamentUI tournamentPanel;

    ChallengesUI challengesUI;

    os::Panel tournamentSidePanel;
    os::Panel roomSidePanel;
    os::Button matchButton;
    os::Label matchQueueing;
    os::Label matchPlaying;

    os::Panel createTournamentPanel;
    os::Edit tournamentName;
    os::Edit sets;
    os::Edit rounds;

    sf::Time updateRoomListTime, updateTournamentListTime;

    OnlineplayUI(sf::Rect<int> _pos, Resources& _res);

    void opTabSelect(const std::string& tab);
    void hideAllPanels(bool keepTournamentOpen = false);

    void createRoom(const std::string& name, const std::string& maxplayers);

    void createRoomPressed();

    void createTournamentPressed();

    void createTournament();

    void matchmakingPressed();

    void back();

    void setRoomListTime();
};

#endif