#include "TournamentUI.h"
#include "gui.h"
#include "network.h"
#include "OnlineplayUI.h"

using std::to_string;
using std::cout;
using std::endl;

void TournamentUI::create(sf::Rect<int> _pos, UI* _gui, tgui::Panel::Ptr parentPanel) {
	createBase(_pos, _gui, parentPanel);

	signUp = tgui::Panel::create();
	signUp->setPosition(0,0);
	signUp->setSize(960,500);
	signUp->setBackgroundColor(sf::Color(255,255,255,0));
	signUp->hide();
	panel->add(signUp);

	tgui::Label::Ptr widget0 = gui->themeTG->load("Label");
	widget0->setPosition(118,24);
	widget0->setText("Tournament info");
	widget0->setTextSize(41);
	signUp->add(widget0);

	tgui::Label::Ptr widget1 = gui->themeTG->load("Label");
	widget1->setPosition(30,95);
	widget1->setText("Starting time");
	widget1->setTextSize(18);
	signUp->add(widget1);

	tStartingTime = gui->themeTG->load("EditBox");
	tStartingTime->setPosition(165,90);
	tStartingTime->setSize(270,30);
	tStartingTime->disable();
	signUp->add(tStartingTime);

	tgui::Label::Ptr widget3 = gui->themeTG->load("Label");
	widget3->setPosition(546,38);
	widget3->setText("Players");
	widget3->setTextSize(18);
	signUp->add(widget3);

	playerList = gui->themeBB->load("ListBox");
	playerList->setPosition(494,75);
	playerList->setSize(180,430);
	playerList->getRenderer()->setTextColor(sf::Color::White);
	signUp->add(playerList);

	tgui::Label::Ptr widget5 = gui->themeTG->load("Label");
	widget5->setPosition(80,145);
	widget5->setText("Rounds");
	widget5->setTextSize(18);
	signUp->add(widget5);

	tgui::Label::Ptr widget6 = gui->themeTG->load("Label");
	widget6->setPosition(295,145);
	widget6->setText("Sets");
	widget6->setTextSize(18);
	signUp->add(widget6);

	tRounds = gui->themeTG->load("EditBox");
	tRounds->setPosition(165,140);
	tRounds->setSize(90,30);
	tRounds->disable();
	signUp->add(tRounds);

	tSets = gui->themeTG->load("EditBox");
	tSets->setPosition(344,140);
	tSets->setSize(90,30);
	tSets->disable();
	signUp->add(tSets);

	signUpButton = gui->themeTG->load("Button");
	signUpButton->setPosition(195,290);
	signUpButton->setSize(210,45);
	signUpButton->setText("Sign up!");
	signUpButton->connect("pressed", &TournamentUI::signUpPressed, this);
	signUp->add(signUpButton);

	tgui::Label::Ptr widget10 = gui->themeTG->load("Label");
	widget10->setPosition(92,195);
	widget10->setText("Status");
	widget10->setTextSize(18);
	signUp->add(widget10);

	tStatus = gui->themeTG->load("EditBox");
	tStatus->setPosition(165,190);
	tStatus->setSize(270,30);
	tStatus->disable();
	signUp->add(tStatus);

	tgui::Label::Ptr widget11 = gui->themeTG->load("Label");
	widget11->setPosition(96,245);
	widget11->setText("Grade");
	widget11->setTextSize(18);
	signUp->add(widget11);

	tGrade = gui->themeTG->load("EditBox");
	tGrade->setPosition(165,240);
	tGrade->setSize(270,30);
	tGrade->disable();
	signUp->add(tGrade);

	closeSign = gui->themeTG->load("Button");
	closeSign->setPosition(195,390);
	closeSign->setSize(210,45);
	closeSign->setText("Close Sign up");
	closeSign->connect("pressed", &TournamentUI::closeSignPressed, this);
	signUp->add(closeSign);

	startTournament = gui->themeTG->load("Button");
	startTournament->setPosition(195,450);
	startTournament->setSize(210,45);
	startTournament->setText("Start tournament");
	startTournament->connect("pressed", &TournamentUI::startTournamentPressed, this);
	signUp->add(startTournament);

	tgui::Button::Ptr signUpBack = gui->themeTG->load("Button");
	signUpBack->setPosition(50,450);
	signUpBack->setSize(100,45);
	signUpBack->setText("Back");
	signUpBack->connect("pressed", &TournamentUI::goBack, this);
	signUp->add(signUpBack);

	bracket = tgui::Panel::create();
	bracket->setPosition(0,0);
	bracket->setSize(960,500);
	bracket->setBackgroundColor(sf::Color(255,255,255,0));
	bracket->hide();
	panel->add(bracket);

	bBack = gui->themeTG->load("Button");
	bBack->setText("Back");
	bBack->setSize(60, 30);
	bBack->connect("pressed", &TournamentUI::goBack, this);

	bStartTournament = gui->themeTG->load("Button");
	bStartTournament->setText("Start!");
	bStartTournament->setSize(60, 30);
	bStartTournament->connect("pressed", &TournamentUI::startTournamentPressed, this);

	bStatus = gui->themeTG->load("Label");
	bracket->add(bStatus);

	bWinner = gui->themeTG->load("Label");

	gameInfo = tgui::Panel::create();
	gameInfo->setPosition(0,0);
	gameInfo->setSize(960,500);
	gameInfo->setBackgroundColor(sf::Color(255,255,255,0));
	gameInfo->hide();
	panel->add(gameInfo);

	gameName = gui->themeTG->load("Label");
	gameName->setPosition(366,17);
	gameName->setText("Game 1");
	gameName->setTextSize(40);
	gameInfo->add(gameName);

	tgui::Label::Ptr widget12 = gui->themeTG->load("Label");
	widget12->setPosition(599,70);
	widget12->setText("Player 1");
	widget12->setTextSize(18);
	gameInfo->add(widget12);

	tgui::Label::Ptr widget13 = gui->themeTG->load("Label");
	widget13->setPosition(803,70);
	widget13->setText("Player 2");
	widget13->setTextSize(18);
	gameInfo->add(widget13);

	player1 = gui->themeTG->load("EditBox");
	player1->setPosition(537,100);
	player1->setSize(200,30);
	player1->disable();
	gameInfo->add(player1);

	player2 = gui->themeTG->load("EditBox");
	player2->setPosition(747,100);
	player2->setSize(200,30);
	player2->disable();
	gameInfo->add(player2);

	tgui::Label::Ptr widget14 = gui->themeTG->load("Label");
	widget14->setPosition(96,103);
	widget14->setText("Starting time");
	widget14->setTextSize(18);
	gameInfo->add(widget14);

	tgui::Label::Ptr widget15 = gui->themeTG->load("Label");
	widget15->setPosition(176,178);
	widget15->setText("Sets");
	widget15->setTextSize(18);
	gameInfo->add(widget15);

	tgui::Label::Ptr widget16 = gui->themeTG->load("Label");
	widget16->setPosition(145,228);
	widget16->setText("Rounds");
	widget16->setTextSize(18);
	gameInfo->add(widget16);

	tgui::Label::Ptr widget17 = gui->themeTG->load("Label");
	widget17->setPosition(156,303);
	widget17->setText("Status");
	widget17->setTextSize(18);
	gameInfo->add(widget17);

	joinButton = gui->themeTG->load("Button");
	joinButton->setPosition(280,340);
	joinButton->setSize(130,35);
	joinButton->setText("Play");
	gameInfo->add(joinButton);

	gSpectate = gui->themeTG->load("Button");
	gSpectate->setPosition(280,340);
	gSpectate->setSize(130, 35);
	gSpectate->setText("Spectate");
	gameInfo->add(gSpectate);

	gStartingTime = gui->themeTG->load("EditBox");
	gStartingTime->setPosition(230,100);
	gStartingTime->setSize(240,30);
	gStartingTime->disable();
	gameInfo->add(gStartingTime);

	gSets = gui->themeTG->load("EditBox");
	gSets->setPosition(230,175);
	gSets->setSize(240,30);
	gSets->disable();
	gameInfo->add(gSets);

	gRounds = gui->themeTG->load("EditBox");
	gRounds->setPosition(230,225);
	gRounds->setSize(240,30);
	gRounds->disable();
	gameInfo->add(gRounds);

	gStatus = gui->themeTG->load("EditBox");
	gStatus->setPosition(230,300);
	gStatus->setSize(240,30);
	gStatus->disable();
	gameInfo->add(gStatus);

	tgui::Label::Ptr widget19 = gui->themeTG->load("Label");
	widget19->setPosition(709,153);
	widget19->setText("Results");
	widget19->setTextSize(18);
	gameInfo->add(widget19);

	for (int i=0; i<7; i++) {
		resultLabel[i] = gui->themeTG->load("Label");
		resultLabel[i]->setTextSize(18);
		resultLabel[i]->setPosition(580,190 + i*40);
		resultLabel[i]->setText("Set " + to_string(i+1));
		gameInfo->add(resultLabel[i]);
	}

	for (int i=0; i<7; i++) {
		p1[i] = gui->themeTG->load("EditBox");
		p1[i]->setPosition(650,187 + i*40);
		p1[i]->setSize(70,30);
		p1[i]->disable();
		gameInfo->add(p1[i]);
	}

	for (int i=0; i<7; i++) {
		p2[i] = gui->themeTG->load("EditBox");
		p2[i]->setPosition(760,187 + i*40);
		p2[i]->setSize(70,30);
		p2[i]->disable();
		gameInfo->add(p2[i]);
	}

	tgui::Button::Ptr widget27 = gui->themeTG->load("Button");
	widget27->setPosition(280,400);
	widget27->setSize(130, 35);
	widget27->setText("Back");
	widget27->connect("pressed", &TournamentUI::goBack, this);
	gameInfo->add(widget27);
}

void TournamentUI::getInfo(sf::Uint16 _myId) {
	myId = _myId;
	double timetostart;
	gui->net.packet >> id >> grade >> rounds >> sets >> timetostart;
	startingTime = timetostart;
	status=0;
	getParticipants();
	getModerators();
	getStatus();
	gRounds->setText(to_string(rounds));
	gSets->setText(to_string(sets));

	if (grade == 1)
		tGrade->setText("Grade A");
	else if (grade == 2)
		tGrade->setText("Grade B");
	else if (grade == 3)
		tGrade->setText("Grade C");
	else if (grade == 4)
		tGrade->setText("Grade D");
	else
		tGrade->setText("Grade E");

	gui->onlineplayUI->hideAllPanels(true);
	show();
	gameInfo->hide();
	bracket->show();
	if (status == 0 || status == 4) {
		signUp->show();
		bracket->hide();
		gameInfo->hide();
		if (startingTime)
			tStartingTime->setText(asctime(localtime(&startingTime)));
		else
			tStartingTime->setText("started by moderator");
		tRounds->setText(to_string(rounds));
		tSets->setText(to_string(sets));
		if (status == 0)
			tStatus->setText("Sign up open");
		else
			tStatus->setText("Aborted");
	}
	else
		getBracket();
}

void TournamentUI::getUpdate(sf::Uint16 _myId) {
	myId = _myId;
	sf::Uint16 tournamentId;
	sf::Uint8 part;
	gui->net.packet >> tournamentId >> part;
	if (tournamentId != id)
		return;
	if (part == 0)
		getParticipants();
	else if (part == 1)
		getModerators();
	else if (part == 2)
		getStatus();
	else if (part == 3) {
		getStatus();
		getBracket();
	}
	else if (part == 4) {
		sf::Uint16 gameId;
		gui->net.packet >> gameId;
		for (auto&& game : games)
			if (game.id == gameId) {
				gui->net.packet >> game.status;
				getNewGameNames(game);
				getResult(game);
				setButtonColors();
				if (game.gameName->getText() == gameName->getText()) {
					setGameStatus(game);
					setGameResults(game);
				}
				return;
			}
	}
}

void TournamentUI::getNewGameNames(TGame& game) {
	sf::Uint16 newid;
	gui->net.packet >> newid;
	if (newid) {
		game.player1_id = newid;
		gui->net.packet >> game.player1_name;
		game.player1->setText(game.player1_name);
	}

	gui->net.packet >> newid;
	if (newid) {
		game.player2_id = newid;
		gui->net.packet >> game.player2_name;
		game.player2->setText(game.player2_name);
	}
}

void TournamentUI::getParticipants() {
	gui->net.packet >> players;
	playerList->removeAllItems();
	participants.clear();
	signUpButton->setText("Sign Up!");
	for (sf::Uint16 i=0; i<players; i++) {
		Participant newplayer;
		gui->net.packet >> newplayer.id >> newplayer.name;
		participants.push_back(newplayer);
		playerList->addItem(newplayer.name);
		if (newplayer.id == myId)
			signUpButton->setText("Withdraw");
	}

	setStatusText();
}

void TournamentUI::getModerators() {
	sf::Uint8 mod_count;
	sf::Uint16 mod_id;
	gui->net.packet >> mod_count;
	moderator = false;
	for (sf::Uint8 i = 0; i < mod_count; i++) {
		gui->net.packet >> mod_id;
		if (mod_id == myId)
			moderator = true;
	}
}

void TournamentUI::getStatus() {
	gui->net.packet >> status;
	setStatusText();
	setModeratorButtons();
}

void TournamentUI::getBracket() {
	TGame newgame;
	games.clear();
	while (!gui->net.packet.endOfPacket()) {
		gui->net.packet >> newgame.id >> newgame.depth >> newgame.status;
		gui->net.packet >> newgame.player1_type;
		if (newgame.player1_type == 1)
			gui->net.packet >> newgame.player1_id >> newgame.player1_name;
		else if (newgame.player1_type == 2) {
			gui->net.packet >> newgame.player1_id;
			newgame.player1_name = "Game " + to_string(newgame.player1_id);
		}
		else if (newgame.player1_type == 3) {
			newgame.player1_id = 0;
			newgame.player1_name = "--Empty--";
			newgame.result.finalScore = "Auto-win";
		}

		gui->net.packet >> newgame.player2_type;
		if (newgame.player2_type == 1)
			gui->net.packet >> newgame.player2_id >> newgame.player2_name;
		else if (newgame.player2_type == 2) {
			gui->net.packet >> newgame.player2_id;
			newgame.player2_name = "Game " + to_string(newgame.player2_id);
		}
		else if (newgame.player2_type == 3) {
			newgame.player2_id = 0;
			newgame.player2_name = "--Empty--";
			newgame.result.finalScore = "Auto-win";
		}

		if (newgame.status > 2)
			getResult(newgame);

		games.push_back(newgame);
	}
	makeBracket();
	if (signUp->isVisible()) {
		signUp->hide();
		bracket->show();
		gameInfo->hide();
	}
}

void TournamentUI::getResult(TGame& game) {
	gui->net.packet >> game.result.p1_sets >> game.result.p2_sets;

	game.result.p1_rounds.clear();
	sf::Uint8 setcount, roundcount;
	gui->net.packet >> setcount;
	for (sf::Uint8 i=0; i<setcount; i++) {
		gui->net.packet >> roundcount;
		game.result.p1_rounds.push_back(roundcount);
	}

	game.result.p2_rounds.clear();
	gui->net.packet >> setcount;
	for (sf::Uint8 i=0; i<setcount; i++) {
		gui->net.packet >> roundcount;
		game.result.p2_rounds.push_back(roundcount);
	}
}

void TournamentUI::makeBracket() {
	bracket->removeAllWidgets();
	int bracketHeight;
	int bracketWidth;
	if (games.size() <=16) {
		bracketHeight = ((games.size()+1)/2)*75;
		bracketWidth = games.back().depth*200;
	}
	else {
		//make big bracket instead
		bracketHeight = ((games.size()+1)/2)*75;
		bracketWidth = games.back().depth*200;
	}
	float wrat = 960.0f / (float)bracketWidth;
	float hrat = 500.0f / (float)bracketHeight;

	//cout << bracketWidth << " " << bracketHeight << endl;
	//cout << wrat << " " << hrat << endl;

	int row = 1, amount_in_row = 1, count = 0;
	for (auto&& game : games) {
		game.button = gui->themeTG->load("Button");
		game.button->setSize(150, 50);
		int buttonx;
		int buttony;
		if (games.size() <=16) {
			buttonx = bracketWidth - 200*row + 25;
			buttony = count*75 + 20 + ((bracketHeight/2) - amount_in_row*37.5);
		}
		else {
			//make big bracket instead
			buttonx = bracketWidth - 200*row + 25;
			buttony = count*75 + 20 + ((bracketHeight/2) - amount_in_row*37.5);
		}
		//cout << buttonx << " - " << buttony << endl;
		game.button->setPosition(buttonx, buttony);
		game.button->setText("");
		game.button->connect("pressed", &TournamentUI::gamePressed, this, std::ref(game));
		bracket->add(game.button);

		game.player1 = gui->themeTG->load("Label");
		game.player1->setPosition(buttonx+5, buttony);
		game.player1->setSize(140, 25);
		game.player1->setText(game.player1_name);
		game.player1->disable(false);
		bracket->add(game.player1);

		game.player2 = gui->themeTG->load("Label");
		game.player2->setPosition(buttonx+5, buttony+25);
		game.player2->setSize(140, 25);
		game.player2->setText(game.player2_name);
		game.player2->disable(false);
		bracket->add(game.player2);

		sf::String _gameName = "Game " + to_string(game.id);
		game.gameName = gui->themeTG->load("Label");
		game.gameName->setText(_gameName);
		game.gameName->setPosition(buttonx + 10, buttony - 22);
		game.gameName->setSize(140, 20);
		bracket->add(game.gameName);
		count++;
		if (count == amount_in_row) {
			row++;
			amount_in_row*=2;
			count=0;
		}

		if (game.status == 0) {
			game.button->hide();
			game.player1->hide();
			game.player2->hide();
			game.gameName->hide();
		}
	}

	setButtonColors();

	if (hrat > wrat) {
		for (auto& widget : bracket->getWidgets()) {
			widget->setPosition(widget->getPosition().x * wrat, widget->getPosition().y * wrat);
			widget->scale(wrat, wrat);
			if (widget->getWidgetType() == "Label")
				std::static_pointer_cast<tgui::Label>(widget)->setTextSize(20*wrat);
		}
	}
	else {
		for (auto& widget : bracket->getWidgets()) {
			widget->setPosition(widget->getPosition().x * hrat, widget->getPosition().y * hrat);
			widget->scale(hrat, hrat);
			if (widget->getWidgetType() == "Label")
				std::static_pointer_cast<tgui::Label>(widget)->setTextSize(20*hrat);
		}
	}

	bWinner->setPosition(360, 360);
	bWinner->setTextSize(32);
	bWinner->hide();
	bracket->add(bWinner);

	setStatusText();
	bracket->add(bStatus);

	bBack->setPosition(bStatus->getPosition().x - 80, bStatus->getPosition().y - 10);
	bracket->add(bBack);

	bStartTournament->setPosition(bBack->getPosition().x - 80, bBack->getPosition().y);
	bStartTournament->setText("Start!");
	bracket->add(bStartTournament);
}

void TournamentUI::setButtonColors() {
	for (auto&& game : games) {
		if (game.player1_id == myId || game.player2_id == myId)
			game.button->getRenderer()->setBorderColor(sf::Color(200, 200, 50));
		if (game.status == 0 || game.status == 4) {
			game.button->getRenderer()->setBackgroundColorNormal(sf::Color(200,50,50,150));
			game.button->getRenderer()->setBackgroundColorHover(sf::Color(200,50,50,100));
			game.button->getRenderer()->setBackgroundColorDown(sf::Color(200,50,50,150));
		}
		else if (game.status == 2) {
			game.button->getRenderer()->setBackgroundColorNormal(sf::Color(50,150,50,150));
			game.button->getRenderer()->setBackgroundColorHover(sf::Color(50,150,50,100));
			game.button->getRenderer()->setBackgroundColorDown(sf::Color(50,150,50,150));
		}
		else if (game.status == 3) {
			game.button->getRenderer()->setBackgroundColorNormal(sf::Color(50,200,50,150));
			game.button->getRenderer()->setBackgroundColorHover(sf::Color(50,200,50,100));
			game.button->getRenderer()->setBackgroundColorDown(sf::Color(50,200,50,150));
		}
	}
}

void TournamentUI::setStatusText() {
	sf::String statustext = "Players: " + to_string(players) + " Status: ";
	if (status == 1)
		statustext += "Pending";
	else if (status == 2)
		statustext += "Started";
	else if (status == 3) {
		statustext += "Finished";
		if (!games.empty()) {
			if (games.front().result.p1_sets > games.front().result.p2_sets)
				bWinner->setText("Winner: " + games.front().player1_name);
			else
				bWinner->setText("Winner: " + games.front().player2_name);
			bWinner->show();
		}
	}
	else if (status == 4)
		statustext += "Aborted";
	else
		statustext += "?";

	if (status == 1)
		signUpButton->show();
	else
		signUpButton->hide();
	bStatus->setText(statustext);
	bStatus->setPosition(bracket->getSize().x-bStatus->getSize().x-10, bracket->getSize().y-30);

	for (auto&& item : gui->onlineplayUI->tournamentList.items)
		if (item.id == id) {
			sf::String label;
			if (status == 0)
				label = "Sign Up - ";
			else if (status == 1)
				label = "Pending - ";
			else if (status == 2)
				label = "Started - ";
			else if (status == 3)
				label = "Finished - ";
			else if (status == 4)
				label = "Aborted - ";
			else
				label = "? - ";
			label += to_string(players) + " players";
			item.label->setText(label);
			return;
		}
}

void TournamentUI::setModeratorButtons() {
	if (moderator && status < 2 && startingTime == 0) {
		closeSign->show();
		startTournament->show();
		bStartTournament->show();
	}
	else {
		bStartTournament->hide();
		closeSign->hide();
		startTournament->hide();
	}
}

void TournamentUI::signUpPressed() {
	if (signUpButton->getText() == "Sign Up!")
		gui->net.sendSignal(9, id);
	else
		gui->net.sendSignal(10, id);
}

void TournamentUI::closeSignPressed() {
	gui->net.sendSignal(11, id);
}

void TournamentUI::startTournamentPressed() {
	gui->net.sendSignal(12, id);
}

void TournamentUI::gamePressed(TGame& game) {
	sf::String nameofgame = game.gameName->getText();
	gameName->setText(nameofgame);
	player1->setText(game.player1_name);
	player2->setText(game.player2_name);
	gStartingTime->setText("ASAP");
	
	setGameStatus(game);

	joinButton->disconnectAll();
	joinButton->connect("pressed", &TournamentUI::playPressed, this, std::ref(game));
	gSpectate->disconnectAll();
	gSpectate->connect("pressed", &TournamentUI::spectate, this, std::ref(game));
	if ((game.status == 2 || game.status == 3) && (game.player1_id == myId || game.player2_id == myId)) {
		joinButton->show();
		gSpectate->hide();
	}
	else {
		joinButton->hide();
		gSpectate->show();
	}

	setGameResults(game);

	bracket->hide();
	gameInfo->show();
}

void TournamentUI::setGameStatus(TGame& game) {
	if (game.status == 0)
		gStatus->setText("Auto-win");
	else if (game.status == 1)
		gStatus->setText("Pending");
	else if (game.status == 2)
		gStatus->setText("Ready to start");
	else if (game.status == 3)
		gStatus->setText("Started");
	else if (game.status == 4)
		gStatus->setText("Finished");
	else
		gStatus->setText("?");
}

void TournamentUI::setGameResults(TGame& game) {
	for (int i=0; i<7; i++) {
		p1[i]->hide();
		p2[i]->hide();
		resultLabel[i]->hide();
	}

	for (unsigned int i=0; i < game.result.p1_rounds.size(); i++) {
		p1[i]->setText(to_string(game.result.p1_rounds[i]));
		p1[i]->show();
		p2[i]->setText(to_string(game.result.p2_rounds[i]));
		p2[i]->show();
		resultLabel[i]->show();
	}
}

void TournamentUI::playPressed(TGame& game) {
	gui->net.sendSignal(13, id, game.id);
}

void TournamentUI::goBack() {
	if (gameInfo->isVisible()) {
		gameInfo->hide();
		bracket->show();
	}
	else {
		hide();
		gui->onlineplayUI->tournamentList.show();
		gui->onlineplayUI->tournamentSidePanel->show();
	}
}

void TournamentUI::spectate(TGame& game) {
	gui->net.sendSignal(19, id, game.id);
}

void TournamentUI::hide() {
	gui->net.sendSignal(14, id);
	panel->hide();
}