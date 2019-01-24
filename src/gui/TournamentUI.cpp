#include "TournamentUI.h"
#include <SFML/Network.hpp>
#include "GameSignals.h"
#include "OnlineplayUI.h"
#include "Resources.h"

TournamentUI::TournamentUI(sf::Rect<int> _pos, Resources& _res, os::Panel& parentPanel, OnlineplayUI& _opui) : GuiBase(_pos, _res, parentPanel), onlineplayUI(_opui) {
    signUp = tgui::Panel::create();
    signUp->setPosition(0, 0);
    signUp->setSize(960, 500);
    signUp->setVisible(false);
    panel->add(signUp);

    tgui::Label::Ptr widget0 = tgui::Label::create();
    widget0->setPosition(118, 24);
    widget0->setText("Tournament info");
    widget0->setTextSize(41);
    signUp->add(widget0);

    tgui::Label::Ptr widget1 = tgui::Label::create();
    widget1->setPosition(30, 95);
    widget1->setText("Starting time");
    widget1->setTextSize(18);
    signUp->add(widget1);

    tStartingTime = tgui::EditBox::create();
    tStartingTime->setPosition(165, 90);
    tStartingTime->setSize(270, 30);
    tStartingTime->setEnabled(false);
    signUp->add(tStartingTime);

    tgui::Label::Ptr widget3 = tgui::Label::create();
    widget3->setPosition(546, 38);
    widget3->setText("Players");
    widget3->setTextSize(18);
    signUp->add(widget3);

    playerList = tgui::ListBox::create();
    playerList->setPosition(494, 75);
    playerList->setSize(180, 430);
    playerList->getRenderer()->setTextColor(sf::Color::White);
    signUp->add(playerList);

    tgui::Label::Ptr widget5 = tgui::Label::create();
    widget5->setPosition(80, 145);
    widget5->setText("Rounds");
    widget5->setTextSize(18);
    signUp->add(widget5);

    tgui::Label::Ptr widget6 = tgui::Label::create();
    widget6->setPosition(295, 145);
    widget6->setText("Sets");
    widget6->setTextSize(18);
    signUp->add(widget6);

    tRounds = tgui::EditBox::create();
    tRounds->setPosition(165, 140);
    tRounds->setSize(90, 30);
    tRounds->setEnabled(false);
    signUp->add(tRounds);

    tSets = tgui::EditBox::create();
    tSets->setPosition(344, 140);
    tSets->setSize(90, 30);
    tSets->setEnabled(false);
    signUp->add(tSets);

    signUpButton = tgui::Button::create();
    signUpButton->setPosition(195, 290);
    signUpButton->setSize(210, 45);
    signUpButton->setText("Sign up!");
    signUpButton->connect("pressed", &TournamentUI::signUpPressed, this);
    signUp->add(signUpButton);

    tgui::Label::Ptr widget10 = tgui::Label::create();
    widget10->setPosition(92, 195);
    widget10->setText("Status");
    widget10->setTextSize(18);
    signUp->add(widget10);

    tStatus = tgui::EditBox::create();
    tStatus->setPosition(165, 190);
    tStatus->setSize(270, 30);
    tStatus->setEnabled(false);
    signUp->add(tStatus);

    tgui::Label::Ptr widget11 = tgui::Label::create();
    widget11->setPosition(96, 245);
    widget11->setText("Grade");
    widget11->setTextSize(18);
    signUp->add(widget11);

    tGrade = tgui::EditBox::create();
    tGrade->setPosition(165, 240);
    tGrade->setSize(270, 30);
    tGrade->setEnabled(false);
    signUp->add(tGrade);

    closeSign = tgui::Button::create();
    closeSign->setPosition(195, 390);
    closeSign->setSize(210, 45);
    closeSign->setText("Close Sign up");
    closeSign->connect("pressed", &TournamentUI::closeSignPressed, this);
    signUp->add(closeSign);

    startTournament = tgui::Button::create();
    startTournament->setPosition(195, 450);
    startTournament->setSize(210, 45);
    startTournament->setText("Start tournament");
    startTournament->connect("pressed", &TournamentUI::startTournamentPressed, this);
    signUp->add(startTournament);

    tgui::Button::Ptr signUpBack = tgui::Button::create();
    signUpBack->setPosition(50, 450);
    signUpBack->setSize(100, 45);
    signUpBack->setText("Back");
    signUpBack->connect("pressed", &TournamentUI::goBack, this);
    signUp->add(signUpBack);

    bracket = tgui::Panel::create();
    bracket->setPosition(0, 0);
    bracket->setSize(960, 500);
    bracket->setVisible(false);
    panel->add(bracket);

    bBack = tgui::Button::create();
    bBack->setText("Back");
    bBack->setSize(60, 30);
    bBack->connect("pressed", &TournamentUI::goBack, this);

    bStartTournament = tgui::Button::create();
    bStartTournament->setText("Start!");
    bStartTournament->setSize(60, 30);
    bStartTournament->connect("pressed", &TournamentUI::startTournamentPressed, this);

    bStatus = tgui::Label::create();
    bracket->add(bStatus);

    bWinner = tgui::Label::create();

    gameInfo = tgui::Panel::create();
    gameInfo->setPosition(0, 0);
    gameInfo->setSize(960, 500);
    gameInfo->setVisible(false);
    panel->add(gameInfo);

    gameName = tgui::Label::create();
    gameName->setPosition(366, 17);
    gameName->setText("Game 1");
    gameName->setTextSize(40);
    gameInfo->add(gameName);

    tgui::Label::Ptr widget12 = tgui::Label::create();
    widget12->setPosition(599, 70);
    widget12->setText("Player 1");
    widget12->setTextSize(18);
    gameInfo->add(widget12);

    tgui::Label::Ptr widget13 = tgui::Label::create();
    widget13->setPosition(803, 70);
    widget13->setText("Player 2");
    widget13->setTextSize(18);
    gameInfo->add(widget13);

    player1 = tgui::EditBox::create();
    player1->setPosition(537, 100);
    player1->setSize(200, 30);
    player1->setEnabled(false);
    gameInfo->add(player1);

    player2 = tgui::EditBox::create();
    player2->setPosition(747, 100);
    player2->setSize(200, 30);
    player2->setEnabled(false);
    gameInfo->add(player2);

    tgui::Label::Ptr widget14 = tgui::Label::create();
    widget14->setPosition(96, 103);
    widget14->setText("Starting time");
    widget14->setTextSize(18);
    gameInfo->add(widget14);

    tgui::Label::Ptr widget15 = tgui::Label::create();
    widget15->setPosition(176, 178);
    widget15->setText("Sets");
    widget15->setTextSize(18);
    gameInfo->add(widget15);

    tgui::Label::Ptr widget16 = tgui::Label::create();
    widget16->setPosition(145, 228);
    widget16->setText("Rounds");
    widget16->setTextSize(18);
    gameInfo->add(widget16);

    tgui::Label::Ptr widget17 = tgui::Label::create();
    widget17->setPosition(156, 303);
    widget17->setText("Status");
    widget17->setTextSize(18);
    gameInfo->add(widget17);

    joinButton = tgui::Button::create();
    joinButton->setPosition(280, 340);
    joinButton->setSize(130, 35);
    joinButton->setText("Play");
    gameInfo->add(joinButton);

    gSpectate = tgui::Button::create();
    gSpectate->setPosition(280, 340);
    gSpectate->setSize(130, 35);
    gSpectate->setText("Spectate");
    gameInfo->add(gSpectate);

    gStartingTime = tgui::EditBox::create();
    gStartingTime->setPosition(230, 100);
    gStartingTime->setSize(240, 30);
    gStartingTime->setEnabled(false);
    gameInfo->add(gStartingTime);

    gSets = tgui::EditBox::create();
    gSets->setPosition(230, 175);
    gSets->setSize(240, 30);
    gSets->setEnabled(false);
    gameInfo->add(gSets);

    gRounds = tgui::EditBox::create();
    gRounds->setPosition(230, 225);
    gRounds->setSize(240, 30);
    gRounds->setEnabled(false);
    gameInfo->add(gRounds);

    gStatus = tgui::EditBox::create();
    gStatus->setPosition(230, 300);
    gStatus->setSize(240, 30);
    gStatus->setEnabled(false);
    gameInfo->add(gStatus);

    tgui::Label::Ptr widget19 = tgui::Label::create();
    widget19->setPosition(709, 153);
    widget19->setText("Results");
    widget19->setTextSize(18);
    gameInfo->add(widget19);

    for (int i = 0; i < 7; i++) {
        resultLabel[i] = tgui::Label::create();
        resultLabel[i]->setTextSize(18);
        resultLabel[i]->setPosition(580, 190 + i * 40);
        resultLabel[i]->setText("Set " + std::to_string(i + 1));
        gameInfo->add(resultLabel[i]);
    }

    for (int i = 0; i < 7; i++) {
        p1[i] = tgui::EditBox::create();
        p1[i]->setPosition(650, 187 + i * 40);
        p1[i]->setSize(70, 30);
        p1[i]->setEnabled(false);
        gameInfo->add(p1[i]);
    }

    for (int i = 0; i < 7; i++) {
        p2[i] = tgui::EditBox::create();
        p2[i]->setPosition(760, 187 + i * 40);
        p2[i]->setSize(70, 30);
        p2[i]->setEnabled(false);
        gameInfo->add(p2[i]);
    }

    tgui::Button::Ptr widget27 = tgui::Button::create();
    widget27->setPosition(280, 400);
    widget27->setSize(130, 35);
    widget27->setText("Back");
    widget27->connect("pressed", &TournamentUI::goBack, this);
    gameInfo->add(widget27);

    Net::takePacket(23, &TournamentUI::getInfo, this);
    Net::takePacket(27, &TournamentUI::getUpdate, this);
}

void TournamentUI::getInfo(sf::Packet& packet) {
    double timetostart;
    packet >> id >> grade >> rounds >> sets >> timetostart;
    startingTime = timetostart;
    status = 0;
    getParticipants(packet);
    getModerators(packet);
    getStatus(packet);
    gRounds->setText(std::to_string(rounds));
    gSets->setText(std::to_string(sets));

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

    onlineplayUI.hideAllPanels(true);
    show();
    gameInfo->setVisible(false);
    bracket->setVisible(true);
    if (status == 0 || status == 4) {
        signUp->setVisible(true);
        bracket->setVisible(false);
        gameInfo->setVisible(false);
        if (startingTime)
            tStartingTime->setText(asctime(localtime(&startingTime)));
        else
            tStartingTime->setText("started by moderator");
        tRounds->setText(std::to_string(rounds));
        tSets->setText(std::to_string(sets));
        if (status == 0)
            tStatus->setText("Sign up open");
        else
            tStatus->setText("Aborted");
    } else
        getBracket(packet);
}

void TournamentUI::getUpdate(sf::Packet& packet) {
    uint16_t tournamentId;
    uint8_t part;
    packet >> tournamentId >> part;
    if (tournamentId != id) return;
    if (part == 0)
        getParticipants(packet);
    else if (part == 1)
        getModerators(packet);
    else if (part == 2)
        getStatus(packet);
    else if (part == 3) {
        getStatus(packet);
        getBracket(packet);
    } else if (part == 4) {
        uint16_t gameId;
        packet >> gameId;
        for (auto&& game : games)
            if (game.id == gameId) {
                packet >> game.status;
                getNewGameNames(game, packet);
                getResult(game, packet);
                setButtonColors();
                if (game.gameName->getText() == gameName->getText()) {
                    setGameStatus(game);
                    setGameResults(game);
                }
                return;
            }
    }
}

void TournamentUI::getNewGameNames(TGame& game, sf::Packet& packet) {
    uint16_t newid;
    packet >> newid;
    if (newid) {
        game.player1_id = newid;
        packet >> game.player1_name;
        game.player1->setText(game.player1_name);
    }

    packet >> newid;
    if (newid) {
        game.player2_id = newid;
        packet >> game.player2_name;
        game.player2->setText(game.player2_name);
    }
}

void TournamentUI::getParticipants(sf::Packet& packet) {
    packet >> players;
    playerList->removeAllItems();
    participants.clear();
    signUpButton->setText("Sign Up!");
    for (uint16_t i = 0; i < players; i++) {
        Participant newplayer;
        packet >> newplayer.id >> newplayer.name;
        participants.push_back(newplayer);
        playerList->addItem(newplayer.name);
        if (newplayer.id == resources.myId) signUpButton->setText("Withdraw");
    }

    setStatusText();
}

void TournamentUI::getModerators(sf::Packet& packet) {
    uint8_t mod_count;
    uint16_t mod_id;
    packet >> mod_count;
    moderator = false;
    for (uint8_t i = 0; i < mod_count; i++) {
        packet >> mod_id;
        if (mod_id == resources.myId) moderator = true;
    }
}

void TournamentUI::getStatus(sf::Packet& packet) {
    packet >> status;
    setStatusText();
    setModeratorButtons();
}

void TournamentUI::getBracket(sf::Packet& packet) {
    TGame newgame;
    games.clear();
    while (!packet.endOfPacket()) {
        packet >> newgame.id >> newgame.depth >> newgame.status;
        packet >> newgame.player1_type;
        if (newgame.player1_type == 1)
            packet >> newgame.player1_id >> newgame.player1_name;
        else if (newgame.player1_type == 2) {
            packet >> newgame.player1_id;
            newgame.player1_name = "Game " + std::to_string(newgame.player1_id);
        } else if (newgame.player1_type == 3) {
            newgame.player1_id = 0;
            newgame.player1_name = "--Empty--";
            newgame.result.finalScore = "Auto-win";
        }

        packet >> newgame.player2_type;
        if (newgame.player2_type == 1)
            packet >> newgame.player2_id >> newgame.player2_name;
        else if (newgame.player2_type == 2) {
            packet >> newgame.player2_id;
            newgame.player2_name = "Game " + std::to_string(newgame.player2_id);
        } else if (newgame.player2_type == 3) {
            newgame.player2_id = 0;
            newgame.player2_name = "--Empty--";
            newgame.result.finalScore = "Auto-win";
        }

        if (newgame.status > 2) getResult(newgame, packet);

        games.push_back(newgame);
    }
    makeBracket();
    if (signUp->isVisible()) {
        signUp->setVisible(false);
        bracket->setVisible(true);
        gameInfo->setVisible(false);
    }
}

void TournamentUI::getResult(TGame& game, sf::Packet& packet) {
    packet >> game.result.p1_sets >> game.result.p2_sets;

    game.result.p1_rounds.clear();
    uint8_t setcount, roundcount;
    packet >> setcount;
    for (uint8_t i = 0; i < setcount; i++) {
        packet >> roundcount;
        game.result.p1_rounds.push_back(roundcount);
    }

    game.result.p2_rounds.clear();
    packet >> setcount;
    for (uint8_t i = 0; i < setcount; i++) {
        packet >> roundcount;
        game.result.p2_rounds.push_back(roundcount);
    }
}

void TournamentUI::makeBracket() {
    bracket->removeAllWidgets();
    int bracketHeight;
    int bracketWidth;
    if (games.size() <= 16) {
        bracketHeight = ((games.size() + 1) / 2) * 75;
        bracketWidth = games.back().depth * 200;
    } else {
        // make big bracket instead
        bracketHeight = ((games.size() + 1) / 2) * 75;
        bracketWidth = games.back().depth * 200;
    }
    float wrat = 960.0f / (float)bracketWidth;
    float hrat = 500.0f / (float)bracketHeight;

    // cout << bracketWidth << " " << bracketHeight << endl;
    // cout << wrat << " " << hrat << endl;

    int row = 1, amount_in_row = 1, count = 0;
    for (auto&& game : games) {
        game.button = tgui::Button::create();
        game.button->setSize(150, 50);
        int buttonx;
        int buttony;
        if (games.size() <= 16) {
            buttonx = bracketWidth - 200 * row + 25;
            buttony = count * 75 + 20 + ((bracketHeight / 2) - amount_in_row * 37.5);
        } else {
            // make big bracket instead
            buttonx = bracketWidth - 200 * row + 25;
            buttony = count * 75 + 20 + ((bracketHeight / 2) - amount_in_row * 37.5);
        }
        // cout << buttonx << " - " << buttony << endl;
        game.button->setPosition(buttonx, buttony);
        game.button->setText("");
        game.button->connect("pressed", &TournamentUI::gamePressed, this, std::ref(game));
        bracket->add(game.button);

        game.player1 = tgui::Label::create();
        game.player1->setPosition(buttonx + 5, buttony);
        game.player1->setSize(140, 25);
        game.player1->setText(game.player1_name);
        game.player1->setEnabled(false);
        bracket->add(game.player1);

        game.player2 = tgui::Label::create();
        game.player2->setPosition(buttonx + 5, buttony + 25);
        game.player2->setSize(140, 25);
        game.player2->setText(game.player2_name);
        game.player2->setEnabled(false);
        bracket->add(game.player2);

        std::string _gameName = "Game " + std::to_string(game.id);
        game.gameName = tgui::Label::create();
        game.gameName->setText(_gameName);
        game.gameName->setPosition(buttonx + 10, buttony - 22);
        game.gameName->setSize(140, 20);
        bracket->add(game.gameName);
        count++;
        if (count == amount_in_row) {
            row++;
            amount_in_row *= 2;
            count = 0;
        }

        if (game.status == 0) {
            game.button->setVisible(false);
            game.player1->setVisible(false);
            game.player2->setVisible(false);
            game.gameName->setVisible(false);
        }
    }

    setButtonColors();

    if (hrat > wrat) {
        for (auto& widget : bracket->getWidgets()) {
            widget->setPosition(widget->getPosition().x * wrat, widget->getPosition().y * wrat);
            widget->setSize(widget->getSize().x * wrat, widget->getSize().y * wrat);
            if (widget->getWidgetType() == "Label") std::static_pointer_cast<tgui::Label>(widget)->setTextSize(20 * wrat);
        }
    } else {
        for (auto& widget : bracket->getWidgets()) {
            widget->setPosition(widget->getPosition().x * hrat, widget->getPosition().y * hrat);
            widget->setSize(widget->getSize().x * hrat, widget->getSize().y * hrat);
            if (widget->getWidgetType() == "Label") std::static_pointer_cast<tgui::Label>(widget)->setTextSize(20 * hrat);
        }
    }

    bWinner->setPosition(360, 360);
    bWinner->setTextSize(32);
    bWinner->setVisible(false);
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
        if (game.player1_id == resources.myId || game.player2_id == resources.myId) game.button->getRenderer()->setBorderColor(sf::Color(200, 200, 50));
        if (game.status == 0 || game.status == 4) {
            game.button->getRenderer()->setBackgroundColor(sf::Color(200, 50, 50, 150));
            game.button->getRenderer()->setBackgroundColorHover(sf::Color(200, 50, 50, 100));
            game.button->getRenderer()->setBackgroundColorDown(sf::Color(200, 50, 50, 150));
        } else if (game.status == 2) {
            game.button->getRenderer()->setBackgroundColor(sf::Color(50, 150, 50, 150));
            game.button->getRenderer()->setBackgroundColorHover(sf::Color(50, 150, 50, 100));
            game.button->getRenderer()->setBackgroundColorDown(sf::Color(50, 150, 50, 150));
        } else if (game.status == 3) {
            game.button->getRenderer()->setBackgroundColor(sf::Color(50, 200, 50, 150));
            game.button->getRenderer()->setBackgroundColorHover(sf::Color(50, 200, 50, 100));
            game.button->getRenderer()->setBackgroundColorDown(sf::Color(50, 200, 50, 150));
        }
    }
}

void TournamentUI::setStatusText() {
    std::string statustext = "Players: " + std::to_string(players) + " Status: ";
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
            bWinner->setVisible(true);
        }
    } else if (status == 4)
        statustext += "Aborted";
    else
        statustext += "?";

    if (status == 0)
        signUpButton->setVisible(true);
    else
        signUpButton->setVisible(false);
    bStatus->setText(statustext);
    bStatus->setPosition(bracket->getSize().x - bStatus->getSize().x - 10, bracket->getSize().y - 30);

    for (auto&& item : onlineplayUI.tournamentList.items)
        if (item.id == id) {
            std::string label;
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
            label += std::to_string(players) + " players";
            item.label->setText(label);
            return;
        }
}

void TournamentUI::setModeratorButtons() {
    if (moderator && status < 2 && startingTime == 0) {
        closeSign->setVisible(true);
        startTournament->setVisible(true);
        bStartTournament->setVisible(true);
    } else {
        bStartTournament->setVisible(false);
        closeSign->setVisible(false);
        startTournament->setVisible(false);
    }
}

void TournamentUI::signUpPressed() {
    if (signUpButton->getText() == "Sign Up!")
        SendSignal(9, id);
    else
        SendSignal(10, id);
}

void TournamentUI::closeSignPressed() { SendSignal(11, id); }

void TournamentUI::startTournamentPressed() { SendSignal(12, id); }

void TournamentUI::gamePressed(TGame& game) {
    std::string nameofgame = game.gameName->getText();
    gameName->setText(nameofgame);
    player1->setText(game.player1_name);
    player2->setText(game.player2_name);
    gStartingTime->setText("ASAP");

    setGameStatus(game);

    joinButton->disconnectAll();
    joinButton->connect("pressed", &TournamentUI::playPressed, this, std::ref(game));
    gSpectate->disconnectAll();
    gSpectate->connect("pressed", &TournamentUI::spectate, this, std::ref(game));
    if ((game.status == 2 || game.status == 3) && (game.player1_id == resources.myId || game.player2_id == resources.myId)) {
        joinButton->setVisible(true);
        gSpectate->setVisible(false);
    } else {
        joinButton->setVisible(false);
        gSpectate->setVisible(true);
    }

    setGameResults(game);

    bracket->setVisible(false);
    gameInfo->setVisible(true);
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
    for (int i = 0; i < 7; i++) {
        p1[i]->setVisible(false);
        p2[i]->setVisible(false);
        resultLabel[i]->setVisible(false);
    }

    for (unsigned int i = 0; i < game.result.p1_rounds.size(); i++) {
        p1[i]->setText(std::to_string(game.result.p1_rounds[i]));
        p1[i]->setVisible(true);
        p2[i]->setText(std::to_string(game.result.p2_rounds[i]));
        p2[i]->setVisible(true);
        resultLabel[i]->setVisible(true);
    }
}

void TournamentUI::playPressed(TGame& game) { SendSignal(13, id, game.id); }

void TournamentUI::goBack() {
    if (gameInfo->isVisible()) {
        gameInfo->setVisible(false);
        bracket->setVisible(true);
    } else {
        hide();
        onlineplayUI.tournamentList.show();
        onlineplayUI.tournamentSidePanel.show();
    }
}

void TournamentUI::spectate(TGame& game) { SendSignal(19, id, game.id); }

void TournamentUI::hide() {
    SendSignal(14, id);
    panel->setVisible(false);
}
