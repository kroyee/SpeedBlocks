#include "UIGameState.h"
#include "gui.h"
#include "GuiElements.h"
#include "GamePlay.h"

UIBaseState::UIBaseState(UI& _ui, GameStates _state) : state(_state), ui(_ui) {
	ui.gamestate = state;
}
UIBaseState::~UIBaseState() {}

void UIBaseState::set(std::unique_ptr<UIBaseState>& state, GameStates _state) {
	UI& uiRef = state->ui;
	state.reset(nullptr);
	if (_state == GameStates::MainMenu)
		state = std::unique_ptr<UIBaseState>(new UIMainMenu(uiRef));
	else if (_state == GameStates::CountDown)
		state = std::unique_ptr<UIBaseState>(new UICountDown(uiRef));
	else if (_state == GameStates::Game)
		state = std::unique_ptr<UIBaseState>(new UIGame(uiRef));
	else if (_state == GameStates::GameOver)
		state = std::unique_ptr<UIBaseState>(new UIGameOver(uiRef));
	else if (_state == GameStates::Replay)
		state = std::unique_ptr<UIBaseState>(new UIReplay(uiRef));
	else if (_state == GameStates::Practice)
		state = std::unique_ptr<UIBaseState>(new UIPractice(uiRef));
	else if (_state == GameStates::Spectating)
		state = std::unique_ptr<UIBaseState>(new UISpectating(uiRef));
}

UIMainMenu::UIMainMenu(UI& _ui) : UIBaseState(_ui, GameStates::MainMenu) {
	ui.guiElements->gameFieldDrawer.removeAllFields();
	ui.away=false;
	ui.guiElements->scoreScreen.hide();
	ui.guiElements->chatScreen.sendTo("Lobby");
	ui.guiElements->gameFieldDrawer.hide();
	ui.guiElements->gameFieldDrawer.hide();
	ui.guiElements->challengesGameUI.hide();
	ui.guiElements->replayUI.hide();
	ui.guiElements->gameStandings.hide();
	if (ui.resources.playonline)
		ui.guiElements->onlineplayUI.show();
	else
		ui.guiElements->trainingUI.show();
}
UIMainMenu::~UIMainMenu() {
	ui.guiElements->mainMenu.hide();
	ui.guiElements->onlineplayUI.hide();
	ui.guiElements->gameFieldDrawer.show();
	ui.guiElements->gameStandings.hide();
	ui.guiElements->trainingUI.hide();
}


UICountDown::UICountDown(UI& _ui) : UIBaseState(_ui, GameStates::CountDown) {}
UICountDown::~UICountDown() {}


UIGame::UIGame(UI& _ui) : UIBaseState(_ui, GameStates::Game) {}
UIGame::~UIGame() {}


UIGameOver::UIGameOver(UI& _ui) : UIBaseState(_ui, GameStates::GameOver) {}
UIGameOver::~UIGameOver() {}


UIReplay::UIReplay(UI& _ui) : UIBaseState(_ui, GameStates::Replay) {}
UIReplay::~UIReplay() {
	ui.guiElements->replayUI.playPause->setText("Play");
	ui.guiElements->replayUI.pauseTime=sf::seconds(0);
}

static auto& SetAway = Signal<void, bool>::get("SetAway");
UIPractice::UIPractice(UI& _ui) : UIBaseState(_ui, GameStates::Practice) {
	if (ui.away)
		SetAway(false);
}
UIPractice::~UIPractice() {}


UISpectating::UISpectating(UI& _ui) : UIBaseState(_ui, GameStates::Spectating) {}
UISpectating::~UISpectating() {
	SendSignal(20);
}