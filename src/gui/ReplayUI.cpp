#include "ReplayUI.h"
#include "Signal.h"
using std::cout;
using std::endl;
using std::to_string;

ReplayUI::ReplayUI(sf::Rect<int> _pos, Resources& _res) : guiBase(_pos, _res) {

	timeTotal = resources.gfx->themeTG->load("Label");
	timeTotal->setPosition(440,5);
	timeTotal->setText("00:00");
	timeTotal->setTextSize(18);
	panel->add(timeTotal);

	playPause = resources.gfx->themeTG->load("Button");
	playPause->setPosition(116,35);
	playPause->setSize(85,30);
	playPause->setText("Pause");
	playPause->connect("pressed", &ReplayUI::pause, this);
	panel->add(playPause);

	gameForward = resources.gfx->themeTG->load("Button");
	gameForward->setPosition(212,35);
	gameForward->setSize(30,30);
	gameForward->setText(">");
	panel->add(gameForward);

	setForward = resources.gfx->themeTG->load("Button");
	setForward->setPosition(252,35);
	setForward->setSize(40,30);
	setForward->setText(">>");
	panel->add(setForward);

	gameBack = resources.gfx->themeTG->load("Button");
	gameBack->setPosition(75,35);
	gameBack->setSize(30,30);
	gameBack->setText("<");
	panel->add(gameBack);

	setBack = resources.gfx->themeTG->load("Button");
	setBack->setPosition(24,35);
	setBack->setSize(40,30);
	setBack->setText("<<");
	panel->add(setBack);

	timePlayed = resources.gfx->themeTG->load("Label");
	timePlayed->setPosition(0,5);
	timePlayed->setText("00:00");
	timePlayed->setTextSize(18);
	panel->add(timePlayed);

	seekbar = resources.gfx->themeTG->load("ProgressBar");
	seekbar->setPosition(50, 5);
	seekbar->setSize(387, 20);
	seekbar->connect("MousePressed", &ReplayUI::seek, this);
	panel->add(seekbar);

	rounds = resources.gfx->themeTG->load("Label");
	rounds->setPosition(310, 40);
	rounds->setText("Round: 1");
	panel->add(rounds);

	sets = resources.gfx->themeTG->load("Label");
	sets->setPosition(420, 40);
	sets->setText("Set: 1");
	panel->add(sets);
}

void ReplayUI::show(bool showTournamentControls) {
	if (showTournamentControls) {
		sets->show();
		rounds->show();
		setBack->show();
		setForward->show();
		gameBack->show();
		gameForward->show();
		backup=false;
	}
	else {
		sets->hide();
		rounds->hide();
		setBack->hide();
		setForward->hide();
		gameBack->hide();
		gameForward->hide();
		Signals::SetName(Signals::RecGetName());
		backup=true;
	}
	panel->show();
	sf::Time duration = Signals::GetRecDuration();
	seekbar->setMaximum(duration.asSeconds());
	timeTotal->setText(displayTime(duration.asSeconds()+1));
	playPause->setText("Pause");

	if (Signals::IsVisible(8))
		Signals::HideStartChallengeButton();
}

void ReplayUI::hide() {
	if (backup) {
		backup=false;
		Signals::SetName(resources.name);
	}
	panel->hide();
}

void ReplayUI::update() {
	sf::Time recTime = Signals::GetRecTime();
	int value = recTime.asSeconds();
	seekbar->setValue(value);
	timePlayed->setText(displayTime(value));
}

void ReplayUI::seek(sf::Vector2f mouse) {
	sf::Uint32 value = (mouse.x/(float)seekbar->getSize().x)*(float)seekbar->getMaximum();
	seekbar->setValue(value);
	Signals::RecJumpTo(value);
	if (resources.gamestate == GameStates::GameOver) {
		Signals::RecUpdateScreen();
		pauseTime = sf::seconds(value);
	}
}

sf::String ReplayUI::displayTime(sf::Uint16 timeVal) {
	int minutes=0;
	while (timeVal >= 60) {
		timeVal-=60;
		minutes++;
	}
	sf::String string = "";
	if (minutes < 10)
		string = "0";
	string += to_string(minutes) + ":";
	if (timeVal < 10)
		string += "0";
	string += to_string(timeVal);

	return string;
}

void ReplayUI::pause() {
	if (resources.gamestate == GameStates::Replay) {
		Signals::SetGameState(GameStates::GameOver);
		pauseTime = Signals::GetRecTime();
		playPause->setText("Play");
	}
	else {
		Signals::SetGameState(GameStates::Replay);
		Signals::RecJumpTo(pauseTime.asMilliseconds());
		playPause->setText("Pause");
	}
}