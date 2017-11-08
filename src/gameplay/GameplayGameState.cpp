#include "GameplayGameState.h"
#include "gamePlay.h"

GPBaseState::GPBaseState(gamePlay& _game, GameStates _state) : game(_game), state(_state) {}
GPBaseState::~GPBaseState() {}

void GPBaseState::set(std::unique_ptr<GPBaseState>& state, GameStates _state) {
	gamePlay& gpRef = state->game;
	state.reset(nullptr);
	if (_state == GameStates::MainMenu)
		state = std::unique_ptr<GPBaseState>(new GPMainMenu(gpRef));
	else if (_state == GameStates::CountDown)
		state = std::unique_ptr<GPBaseState>(new GPCountDown(gpRef));
	else if (_state == GameStates::Game)
		state = std::unique_ptr<GPBaseState>(new GPGame(gpRef));
	else if (_state == GameStates::GameOver)
		state = std::unique_ptr<GPBaseState>(new GPGameOver(gpRef));
	else if (_state == GameStates::Replay)
		state = std::unique_ptr<GPBaseState>(new GPReplay(gpRef));
	else if (_state == GameStates::Practice)
		state = std::unique_ptr<GPBaseState>(new GPPractice(gpRef));
	else if (_state == GameStates::Spectating)
		state = std::unique_ptr<GPBaseState>(new GPSpectating(gpRef));
}

GPMainMenu::GPMainMenu(gamePlay& _game) : GPBaseState(_game, GameStates::MainMenu) {
	game.field.clear();
	game.autoaway=false;
	game.field.text.away=false;
}
GPMainMenu::~GPMainMenu() {
	game.showPressEnterText=true;
}


GPCountDown::GPCountDown(gamePlay& _game) : GPBaseState(_game, GameStates::CountDown) {
	game.sRKey();
    game.sLKey();
    game.sDKey();
    game.showPressEnterText=false;
    game.draw();
}
GPCountDown::~GPCountDown() {}


GPGame::GPGame(gamePlay& _game) : GPBaseState(_game, GameStates::Game) {
	game.showPressEnterText=false;
    game.startGame();
}
GPGame::~GPGame() {
	game.showPressEnterText=true;
	game.aiManager.endRound(game.gameclock.getElapsedTime());
}


GPGameOver::GPGameOver(gamePlay& _game) : GPBaseState(_game, GameStates::GameOver) {
	if (game.autoaway)
    	Signals::SetAway(true);
    game.field.text.setCountdown(0);
    game.draw();
}
GPGameOver::~GPGameOver() {}


GPReplay::GPReplay(gamePlay& _game) : GPBaseState(_game, GameStates::Replay) {
	game.showPressEnterText=false;
	game.startReplay();
}
GPReplay::~GPReplay() {}


GPPractice::GPPractice(gamePlay& _game) : GPBaseState(_game, GameStates::Practice) {
	if (!game.field.text.ready)
		Signals::Ready();
	game.field.clear();
	game.showPressEnterText=false;
    game.startGame();
}
GPPractice::~GPPractice() {
	game.autoaway = false;
	game.field.text.away=false;
	game.field.text.ready=true;
}


GPSpectating::GPSpectating(gamePlay& _game) : GPBaseState(_game, GameStates::Spectating) {}
GPSpectating::~GPSpectating() {}