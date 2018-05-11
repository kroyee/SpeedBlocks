#include "ReplayUI.h"
#include "GameSignals.h"
#include "Resources.h"

static auto& GetRecName = Signal<const std::string&>::get("GetRecName");
static auto& HideStartChallengeButton = Signal<void>::get("HideStartChallengeButton");
static auto& RecJumpTo = Signal<void, int>::get("RecJumpTo");
static auto& RecUpdateScreen = Signal<void>::get("RecUpdateScreen");
static auto& GetRecTime = Signal<sf::Time>::get("GetRecTime");
static auto& GetRecDuration = Signal<const sf::Time&>::get("GetRecDuration");
static auto& IsVisible = Signal<bool, int>::get("IsVisible");
static auto& SetGameState = Signal<void, GameStates>::get("SetGameState");
static auto& SetName = Signal<void, const std::string&>::get("SetName");

ReplayUI::ReplayUI(sf::Rect<int> _pos, Resources& _res) : GuiBase(_pos, _res) {

	timeTotal = resources.gfx->load("Label");
	timeTotal->setPosition(440,5);
	timeTotal->setText("00:00");
	timeTotal->setTextSize(18);
	panel->add(timeTotal);

	playPause = resources.gfx->load("Button");
	playPause->setPosition(116,35);
	playPause->setSize(85,30);
	playPause->setText("Pause");
	playPause->connect("pressed", &ReplayUI::pause, this);
	panel->add(playPause);

	gameForward = resources.gfx->load("Button");
	gameForward->setPosition(212,35);
	gameForward->setSize(30,30);
	gameForward->setText(">");
	panel->add(gameForward);

	setForward = resources.gfx->load("Button");
	setForward->setPosition(252,35);
	setForward->setSize(40,30);
	setForward->setText(">>");
	panel->add(setForward);

	gameBack = resources.gfx->load("Button");
	gameBack->setPosition(75,35);
	gameBack->setSize(30,30);
	gameBack->setText("<");
	panel->add(gameBack);

	setBack = resources.gfx->load("Button");
	setBack->setPosition(24,35);
	setBack->setSize(40,30);
	setBack->setText("<<");
	panel->add(setBack);

	timePlayed = resources.gfx->load("Label");
	timePlayed->setPosition(0,5);
	timePlayed->setText("00:00");
	timePlayed->setTextSize(18);
	panel->add(timePlayed);

	seekbar = resources.gfx->load("ProgressBar");
	seekbar->setPosition(50, 5);
	seekbar->setSize(387, 20);
	seekbar->connect("MousePressed", &ReplayUI::seek, this);
	panel->add(seekbar);

	rounds = resources.gfx->load("Label");
	rounds->setPosition(310, 40);
	rounds->setText("Round: 1");
	panel->add(rounds);

	sets = resources.gfx->load("Label");
	sets->setPosition(420, 40);
	sets->setText("Set: 1");
	panel->add(sets);

	connectSignal("UpdateReplayUI", &ReplayUI::update, this);
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
		SetName(GetRecName());
		backup=true;
	}
	panel->show();
	sf::Time duration = GetRecDuration();
	seekbar->setMaximum(duration.asSeconds());
	timeTotal->setText(displayTime(duration.asSeconds()+1));
	playPause->setText("Pause");

	if (IsVisible(8))
		HideStartChallengeButton();
}

void ReplayUI::hide() {
	if (backup) {
		backup=false;
		SetName(resources.name);
	}
	panel->hide();
}

void ReplayUI::update(sf::Time recTime) {
	int value = recTime.asSeconds();
	seekbar->setValue(value);
	timePlayed->setText(displayTime(value));
}

void ReplayUI::seek(sf::Vector2f mouse) {
	uint32_t value = (mouse.x/(float)seekbar->getSize().x)*(float)seekbar->getMaximum();
	seekbar->setValue(value);
	RecJumpTo(value*1000);
	if (resources.gamestate == GameStates::GameOver) {
		RecUpdateScreen();
		pauseTime = sf::seconds(value);
	}
}

std::string ReplayUI::displayTime(uint16_t timeVal) {
	int minutes=0;
	while (timeVal >= 60) {
		timeVal-=60;
		minutes++;
	}
	std::string string = "";
	if (minutes < 10)
		string = "0";
	string += std::to_string(minutes) + ":";
	if (timeVal < 10)
		string += "0";
	string += std::to_string(timeVal);

	return string;
}

void ReplayUI::pause() {
	if (resources.gamestate == GameStates::Replay) {
		SetGameState(GameStates::GameOver);
		pauseTime = GetRecTime();
		playPause->setText("Play");
	}
	else {
		SetGameState(GameStates::Replay);
		RecJumpTo(pauseTime.asMilliseconds());
		playPause->setText("Pause");
	}
}
