#ifndef TOURNAMENTUI_H
#define TOURNAMENTUI_H

#include "guiBase.h"

class UI;

struct Result {
	sf::Uint8 p1_sets;
	sf::Uint8 p2_sets;

	std::vector<sf::Uint8> p1_rounds;
	std::vector<sf::Uint8> p2_rounds;

	sf::String finalScore;
};

struct TGame {
	sf::Uint16 id;
	sf::Uint8 depth, status;
	sf::Uint8 player1_type, player2_type;
	sf::Uint16 player1_id, player2_id;
	sf::String player1_name, player2_name;
	time_t startingTime;

	Result result;

	tgui::Button::Ptr button;
	tgui::Label::Ptr gameName;
	tgui::Label::Ptr player1;
	tgui::Label::Ptr player2;
};

struct Participant {
	sf::Uint16 id;
	sf::String name;
};

class TournamentUI : public guiBase {
public:
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

	sf::Uint8 sets, rounds, status, grade;
	sf::Uint16 id, players;
	sf::Uint16 myId;
	bool moderator;
	time_t startingTime;

	void create(sf::Rect<int> _pos, UI* _gui, tgui::Panel::Ptr parentPanel);

	void getInfo(sf::Uint16 _myId);
	void getUpdate(sf::Uint16 _myId);
	void getNewGameNames(TGame& game);
	void getParticipants();
	void getModerators();
	void getStatus();
	void getBracket();
	void getResult(TGame& game);
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