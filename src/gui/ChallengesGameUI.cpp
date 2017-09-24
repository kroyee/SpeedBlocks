#include "ChallengesGameUI.h"
#include "Signal.h"
#include "gamePlay.h"
using std::to_string;

ChallengesGameUI::ChallengesGameUI(sf::Rect<int> _pos, Resources& _res) : guiBase(_pos, _res) {

	// Race

	racePanel = tgui::Panel::create();
	racePanel->setPosition(465,40);
	racePanel->setSize(490,555);
	racePanel->setBackgroundColor(sf::Color(255,255,255,0));
	racePanel->hide();
	panel->add(racePanel);

	tgui::Label::Ptr widget0 = resources.gfx->themeTG->load("Label");
	widget0->setPosition(177,11);
	widget0->setText("Race");
	widget0->setTextSize(60);
	racePanel->add(widget0);

	tgui::Label::Ptr widget1 = resources.gfx->themeTG->load("Label");
	widget1->setPosition(61,90);
	widget1->setText("Time Elapsed");
	widget1->setTextSize(30);
	racePanel->add(widget1);

	tgui::Label::Ptr widget2 = resources.gfx->themeTG->load("Label");
	widget2->setPosition(21,160);
	widget2->setText("Lines remaining");
	widget2->setTextSize(30);
	racePanel->add(widget2);

	tgui::Label::Ptr widget3 = resources.gfx->themeTG->load("Label");
	widget3->setPosition(87,230);
	widget3->setText("Blocks used");
	widget3->setTextSize(30);
	racePanel->add(widget3);

	startChallenge = resources.gfx->themeTG->load("Button");
	startChallenge->setPosition(153,311);
	startChallenge->setSize(200,40);
	startChallenge->setText("Start challenge");
	startChallenge->connect("pressed", [&](){ Signals::Ready(); });
	racePanel->add(startChallenge);

	raceTimeElapsed = resources.gfx->themeTG->load("EditBox");
	raceTimeElapsed->setPosition(280,87);
	raceTimeElapsed->setSize(130,40);
	racePanel->add(raceTimeElapsed);

	raceLinesRemaining = resources.gfx->themeTG->load("EditBox");
	raceLinesRemaining->setPosition(280,157);
	raceLinesRemaining->setSize(130,40);
	racePanel->add(raceLinesRemaining);

	raceBlocksUsed = resources.gfx->themeTG->load("EditBox");
	raceBlocksUsed->setPosition(280,227);
	raceBlocksUsed->setSize(130,40);
	racePanel->add(raceBlocksUsed);

	// Cheese

	cheesePanel = tgui::Panel::create();
	cheesePanel->setPosition(465,40);
	cheesePanel->setSize(490,555);
	cheesePanel->setBackgroundColor(sf::Color(255,255,255,0));
	cheesePanel->hide();
	panel->add(cheesePanel);

	tgui::Label::Ptr widget5 = resources.gfx->themeTG->load("Label");
	widget5->setPosition(177,11);
	widget5->setText("Cheese");
	widget5->setTextSize(60);
	cheesePanel->add(widget5);

	tgui::Label::Ptr widget6 = resources.gfx->themeTG->load("Label");
	widget6->setPosition(61,90);
	widget6->setText("Time Elapsed");
	widget6->setTextSize(30);
	cheesePanel->add(widget6);

	tgui::Label::Ptr widget7 = resources.gfx->themeTG->load("Label");
	widget7->setPosition(21,160);
	widget7->setText("Lines remaining");
	widget7->setTextSize(30);
	cheesePanel->add(widget7);

	tgui::Label::Ptr widget8 = resources.gfx->themeTG->load("Label");
	widget8->setPosition(87,230);
	widget8->setText("Blocks used");
	widget8->setTextSize(30);
	cheesePanel->add(widget8);

	cheesePanel->add(startChallenge);

	cheeseTimeElapsed = resources.gfx->themeTG->load("EditBox");
	cheeseTimeElapsed->setPosition(280,87);
	cheeseTimeElapsed->setSize(130,40);
	cheesePanel->add(cheeseTimeElapsed);

	cheeseLinesRemaining = resources.gfx->themeTG->load("EditBox");
	cheeseLinesRemaining->setPosition(280,157);
	cheeseLinesRemaining->setSize(130,40);
	cheesePanel->add(cheeseLinesRemaining);

	cheeseBlocksUsed = resources.gfx->themeTG->load("EditBox");
	cheeseBlocksUsed->setPosition(280,227);
	cheeseBlocksUsed->setSize(130,40);
	cheesePanel->add(cheeseBlocksUsed);

	Signals::HideStartChallengeButton.connect(&ChallengesGameUI::hideStartChallengeButton, this);
}

void ChallengesGameUI::clear() {
	raceTimeElapsed->setText("");
	raceLinesRemaining->setText("");
	raceBlocksUsed->setText("");
	cheeseTimeElapsed->setText("");
	cheeseLinesRemaining->setText("");
	cheeseBlocksUsed->setText("");
}

void ChallengesGameUI::update() {
	/*if (racePanel->isVisible()) {
		if (resources.gamestate == Replay)
			raceTimeElapsed->setText(displayTime(gui->game.recorder.timer.getElapsedTime() + gui->game.recorder.startAt));
		else
			raceTimeElapsed->setText(displayTime(gui->game.gameclock.getElapsedTime()));
		raceLinesRemaining->setText(to_string(40 - gui->game.linesCleared));
		raceBlocksUsed->setText(to_string(gui->game.pieceCount));
		if (gui->game.linesCleared > 39) {
			gui->game.gameover=true;
			gui->game.winner=true;
		}
	}
	else if (cheesePanel->isVisible()) {
		if (resources.gamestate == Replay)
			cheeseTimeElapsed->setText(displayTime(gui->game.recorder.timer.getElapsedTime() + gui->game.recorder.startAt));
		else
			cheeseTimeElapsed->setText(displayTime(gui->game.gameclock.getElapsedTime()));
		cheeseLinesRemaining->setText(to_string(9 - gui->game.garbageCleared));
		cheeseBlocksUsed->setText(to_string(gui->game.pieceCount));
		if (gui->game.garbageCleared > 8) {
			gui->game.gameover=true;
			gui->game.winner=true;
		}
	}*/
}

sf::String ChallengesGameUI::displayTime(const sf::Time& time) {
	sf::String timeString;
	sf::Uint16 total = time.asSeconds();
	sf::Uint16 count = 0;
	while (total > 60) {
		total-=60;
		count++;
	}
	timeString = to_string(count) + ":";
	if (total < 10)
		timeString += "0";
	timeString += to_string(total) + ":";
	count = (time.asMilliseconds() % 1000) / 10;
	if (count < 10)
		timeString += "0";
	timeString += to_string(count);
	return timeString;
}

void ChallengesGameUI::hideAllPanels() {
	racePanel->hide();
	cheesePanel->hide();
}

void ChallengesGameUI::showPanel(sf::Uint16 whichPanel) {
	hideAllPanels();
	startChallenge->show();
	if (whichPanel == 20000)
		racePanel->show();
	else if (whichPanel == 20001)
		cheesePanel->show();
}

void ChallengesGameUI::hideStartChallengeButton() {
	startChallenge->hide();
}