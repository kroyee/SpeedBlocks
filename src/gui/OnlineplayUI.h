#ifndef ONLINEPLAYUI_H
#define ONLINEPLAYUI_H

#include "guiBase.h"
#include "ScrollList.h"
#include "TournamentUI.h"
#include "ChallengesUI.h"

class UI;

class OnlineplayUI : public guiBase {
public:
	tgui::Tab::Ptr opTab;
	tgui::Panel::Ptr CreateRoom;

	ScrollList roomList;
	ScrollList tournamentList;

	TournamentUI tournamentPanel;

	ChallengesUI challengesUI;

	tgui::Panel::Ptr tournamentSidePanel;
	tgui::Panel::Ptr roomSidePanel;
	tgui::Button::Ptr matchButton;
	tgui::Label::Ptr matchQueueing;
	tgui::Label::Ptr matchPlaying;

	tgui::Panel::Ptr createTournamentPanel;
	tgui::EditBox::Ptr tournamentName;
	tgui::EditBox::Ptr sets;
	tgui::EditBox::Ptr rounds;

	sf::Time updateRoomListTime, updateTournamentListTime;

	void create(sf::Rect<int> _pos, UI* _gui);

	void opTabSelect(const std::string& tab);
	void hideAllPanels(bool keepTournamentOpen = false);

	void createRoom(const sf::String& name, const sf::String& maxplayers);
	void makeRoomList();
	void addRoom();

	void createRoomPressed();

	void makeTournamentList();
	void addTournament();

	void createTournamentPressed();

	void createTournament();

	void matchmakingPressed();

	void alertMsg(const sf::Uint16 id1);

	void back();
};

#endif