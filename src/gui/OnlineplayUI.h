#ifndef ONLINEPLAYUI_H
#define ONLINEPLAYUI_H

#include "GuiBase.h"
#include "ScrollList.h"
#include "TournamentUI.h"
#include "ChallengesUI.h"

class Resources;

class OnlineplayUI : public GuiBase {
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

	OnlineplayUI(sf::Rect<int> _pos, Resources& _res);

	void opTabSelect(const std::string& tab);
	void hideAllPanels(bool keepTournamentOpen = false);

	void createRoom(const std::string& name, const std::string& maxplayers);
	void makeRoomList(sf::Packet &packet);
	void addRoom(sf::Packet &packet);

	void createRoomPressed();

	void makeTournamentList(sf::Packet &packet);
	void addTournament(sf::Packet &packet);

	void createTournamentPressed();

	void createTournament();

	void matchmakingPressed();

	void alertMsg(const uint16_t id1);

	void back();

	void setRoomListTime();
};

#endif