#include "ReplayUI.h"
#include "gui.h"
#include "gamePlay.h"
using std::cout;
using std::endl;
using std::to_string;

void ReplayUI::create(sf::Rect<int> _pos, UI* _ui) {
	createBase(_pos, _ui);

	timeTotal = gui->themeTG->load("Label");
	timeTotal->setPosition(440,5);
	timeTotal->setText("00:00");
	timeTotal->setTextSize(18);
	panel->add(timeTotal);

	playPause = gui->themeTG->load("Button");
	playPause->setPosition(116,35);
	playPause->setSize(85,30);
	playPause->setText("Pause");
	playPause->connect("pressed", &ReplayUI::pause, this);
	panel->add(playPause);

	gameForward = gui->themeTG->load("Button");
	gameForward->setPosition(212,35);
	gameForward->setSize(30,30);
	gameForward->setText(">");
	panel->add(gameForward);

	setForward = gui->themeTG->load("Button");
	setForward->setPosition(252,35);
	setForward->setSize(40,30);
	setForward->setText(">>");
	panel->add(setForward);

	gameBack = gui->themeTG->load("Button");
	gameBack->setPosition(75,35);
	gameBack->setSize(30,30);
	gameBack->setText("<");
	panel->add(gameBack);

	setBack = gui->themeTG->load("Button");
	setBack->setPosition(24,35);
	setBack->setSize(40,30);
	setBack->setText("<<");
	panel->add(setBack);

	timePlayed = gui->themeTG->load("Label");
	timePlayed->setPosition(0,5);
	timePlayed->setText("00:00");
	timePlayed->setTextSize(18);
	panel->add(timePlayed);

	seekbar = gui->themeTG->load("ProgressBar");
	seekbar->setPosition(50, 5);
	seekbar->setSize(387, 20);
	seekbar->connect("MousePressed", &ReplayUI::seek, this);
	panel->add(seekbar);

	rounds = gui->themeTG->load("Label");
	rounds->setPosition(310, 40);
	rounds->setText("Round: 1");
	panel->add(rounds);

	sets = gui->themeTG->load("Label");
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
	}
	else {
		sets->hide();
		rounds->hide();
		setBack->hide();
		setForward->hide();
		gameBack->hide();
		gameForward->hide();
	}
	panel->show();
	seekbar->setMaximum(gui->game.recorder.duration.asSeconds());
	timeTotal->setText(displayTime(gui->game.recorder.duration.asSeconds()+1));
	playPause->setText("Pause");
}

void ReplayUI::update() {
	int value = (gui->game.recorder.timer.getElapsedTime() + gui->game.recorder.startAt).asSeconds();
	seekbar->setValue(value);
	timePlayed->setText(displayTime(value));
}

void ReplayUI::seek(sf::Vector2f mouse) {
	int value = (mouse.x/(float)seekbar->getSize().x)*(float)seekbar->getMaximum();
	seekbar->setValue(value);
	gui->game.recorder.jumpTo(value*1000);
	if (gui->gamestate == GameOver) {
		gui->game.playReplay();
		gui->game.draw();
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
	if (gui->gamestate == Replay) {
		gui->setGameState(GameOver);
		pauseTime = gui->game.recorder.timer.getElapsedTime() + gui->game.recorder.startAt;
		playPause->setText("Play");
	}
	else {
		gui->setGameState(Replay);
		gui->game.recorder.jumpTo(pauseTime.asMilliseconds());
		playPause->setText("Pause");
	}
}