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
    timeTotal.pos(440, 5).text("00:00").text_size(18).add_to(panel);

    playPause.pos(116, 35).size(85, 30).text("Pause").connect("pressed", &ReplayUI::pause, this).add_to(panel);

    gameForward.pos(212, 35).size(30, 30).text(">").add_to(panel);

    setForward.pos(252, 35).size(40, 30).text(">>").add_to(panel);

    gameBack.pos(75, 35).size(30, 30).text("<").add_to(panel);

    setBack.pos(24, 35).size(40, 30).text("<<").add_to(panel);

    timePlayed.pos(0, 5).text("00:00").text_size(18).add_to(panel);

    seekbar.pos(50, 5).size(387, 20).connect("MousePressed", &ReplayUI::seek, this).add_to(panel);

    rounds.pos(310, 40).text("Round: 1").add_to(panel);

    sets.pos(420, 40).text("Set: 1").add_to(panel);

    connectSignal("UpdateReplayUI", &ReplayUI::update, this);
}

void ReplayUI::show(bool showTournamentControls) {
    if (showTournamentControls) {
        sets.show();
        rounds.show();
        setBack.show();
        setForward.show();
        gameBack.show();
        gameForward.show();
        backup = false;
    } else {
        sets.hide();
        rounds.hide();
        setBack.hide();
        setForward.hide();
        gameBack.hide();
        gameForward.hide();
        SetName(GetRecName());
        backup = true;
    }
    panel.show();
    sf::Time duration = GetRecDuration();
    seekbar.max(duration.asSeconds());
    timeTotal.text(displayTime(duration.asSeconds() + 1));
    playPause.text("Pause");

    if (IsVisible(8)) HideStartChallengeButton();
}

void ReplayUI::hide() {
    if (backup) {
        backup = false;
        SetName(resources.name);
    }
    panel.hide();
}

void ReplayUI::update(sf::Time recTime) {
    int value = recTime.asSeconds();
    seekbar.set(value);
    timePlayed.text(displayTime(value));
}

void ReplayUI::seek(sf::Vector2f mouse) {
    uint32_t value = (mouse.x / (float)seekbar->getSize().x) * (float)seekbar->getMaximum();
    seekbar.set(value);
    RecJumpTo(value * 1000);
    if (resources.gamestate == GameStates::GameOver) {
        RecUpdateScreen();
        pauseTime = sf::seconds(value);
    }
}

std::string ReplayUI::displayTime(uint16_t timeVal) {
    int minutes = 0;
    while (timeVal >= 60) {
        timeVal -= 60;
        minutes++;
    }
    std::string string = "";
    if (minutes < 10) string = "0";
    string += std::to_string(minutes) + ":";
    if (timeVal < 10) string += "0";
    string += std::to_string(timeVal);

    return string;
}

void ReplayUI::pause() {
    if (resources.gamestate == GameStates::Replay) {
        SetGameState(GameStates::GameOver);
        pauseTime = GetRecTime();
        playPause.text("Play");
    } else {
        SetGameState(GameStates::Replay);
        RecJumpTo(pauseTime.asMilliseconds());
        playPause.text("Pause");
    }
}
