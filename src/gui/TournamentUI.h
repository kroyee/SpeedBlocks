#ifndef TOURNAMENTUI_H
#define TOURNAMENTUI_H

#include "guiBase.h"

class Resources;
class OnlineplayUI;

namespace sf { class Packet; }

struct Result {
	uint8_t p1_sets;
	uint8_t p2_sets;

	std::vector<uint8_t> p1_rounds;
	std::vector<uint8_t> p2_rounds;

	std::string finalScore;
};

struct TGame {
	uint16_t id;
	uint8_t depth, status;
	uint8_t player1_type, player2_type;
	uint16_t player1_id, player2_id;
	std::string player1_name, player2_name;
	time_t startingTime;

	Result result;

	tgui::Button::Ptr button;
	tgui::Label::Ptr gameName;
	tgui::Label::Ptr player1;
	tgui::Label::Ptr player2;
};

struct Participant {
	uint16_t id;
	std::string name;
};

class TournamentUI : public guiBase {
public:
	OnlineplayUI& onlineplayUI;
	std::list<TGame> games;
	std::list<Participant> participants;

	tgui::Panel::Ptr signUp;
	tgui::EditBox::Ptr tStartingTime;
	tgui::ListBox::Ptr playerList;
	tgui::EditBox::Ptr tRounds;
	tgui::EditBox::Ptr tSets;
	tgui::Button::Ptr signUpButton;
	tgui::EditBox::Ptr tStatus;
	tgui::EditBox::Ptr tGrade;
	tgui::Button::Ptr closeSign;
	tgui::Button::Ptr startTournament;

	tgui::Panel::Ptr bracket;
	tgui::Label::Ptr bStatus;
	tgui::Label::Ptr bWinner;
	tgui::Button::Ptr bBack;
	tgui::Button::Ptr bStartTournament;

	tgui::Panel::Ptr gameInfo;
	tgui::Label::Ptr gameName;
	tgui::EditBox::Ptr player1;
	tgui::EditBox::Ptr player2;
	tgui::Button::Ptr joinButton;
	tgui::Button::Ptr gSpectate;
	tgui::EditBox::Ptr gStartingTime;
	tgui::EditBox::Ptr gSets;
	tgui::EditBox::Ptr gRounds;
	tgui::EditBox::Ptr gStatus;
	tgui::Label::Ptr resultLabel[7];
	tgui::EditBox::Ptr p1[7];
	tgui::EditBox::Ptr p2[7];

	uint8_t sets, rounds, status, grade;
	uint16_t id, players;
	bool moderator;
	time_t startingTime;

	TournamentUI(sf::Rect<int> _pos, Resources& _res, tgui::Panel::Ptr parentPanel, OnlineplayUI& _opui);

	void getInfo(sf::Packet &packet);
	void getUpdate(sf::Packet &packet);
	void getNewGameNames(TGame& game, sf::Packet &packet);
	void getParticipants(sf::Packet& packet);
	void getModerators(sf::Packet& packet);
	void getStatus(sf::Packet &packet);
	void getBracket(sf::Packet &packet);
	void getResult(TGame& game, sf::Packet &packet);
	void makeBracket();
	void setButtonColors();
	void setStatusText();
	void setModeratorButtons();
	void setGameStatus(TGame& game);
	void setGameResults(TGame& game);

	void signUpPressed();
	void closeSignPressed();
	void startTournamentPressed();

	void gamePressed(TGame& game);
	void playPressed(TGame& game);
	void goBack();
	void spectate(TGame& game);

	void hide();
};

#endif