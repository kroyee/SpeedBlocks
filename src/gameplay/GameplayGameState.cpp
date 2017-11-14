#include "GameplayGameState.h"
#include "gamePlay.h"

GPBaseState::GPBaseState(gamePlay& _game, GameStates _state) : state(_state), game(_game) {
	game.resources.gamestate = state;
}
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

//////////////////////////////////////////////////////////////////////
//							Main Menu								//
//////////////////////////////////////////////////////////////////////

GPMainMenu::GPMainMenu(gamePlay& _game) : GPBaseState(_game, GameStates::MainMenu) {
	game.field.clear();
	game.autoaway=false;
	game.field.text.away=false;
	Signals::AmountAI(0);
}
GPMainMenu::~GPMainMenu() {
	game.showPressEnterText=true;
}

//////////////////////////////////////////////////////////////////////
//							Countdown								//
//////////////////////////////////////////////////////////////////////

GPCountDown::GPCountDown(gamePlay& _game) : GPBaseState(_game, GameStates::CountDown) {
	game.sRKey();
    game.sLKey();
    game.sDKey();
    game.showPressEnterText=false;
    game.drawMe=true;
}
GPCountDown::~GPCountDown() {}

void GPCountDown::update() {
	if (!game.resources.playonline)
        if (game.countDown())
            Signals::SetGameState(GameStates::Game);
}

//////////////////////////////////////////////////////////////////////
//							Game									//
//////////////////////////////////////////////////////////////////////

GPGame::GPGame(gamePlay& _game) : GPBaseState(_game, GameStates::Game) {
	game.showPressEnterText=false;
    game.startGame();
}
GPGame::~GPGame() {
	game.showPressEnterText=true;
	game.aiManager.endRound(game.gameclock.getElapsedTime());
}

void GPGame::update() {
	game.delayCheck();
}

//////////////////////////////////////////////////////////////////////
//							Game Over								//
//////////////////////////////////////////////////////////////////////

GPGameOver::GPGameOver(gamePlay& _game) : GPBaseState(_game, GameStates::GameOver) {
	if (game.autoaway)
    	Signals::SetAway(true);
    game.field.text.setCountdown(0);
    game.drawMe=true;
}
GPGameOver::~GPGameOver() {}

//////////////////////////////////////////////////////////////////////
//							Replay									//
//////////////////////////////////////////////////////////////////////

GPReplay::GPReplay(gamePlay& _game) : GPBaseState(_game, GameStates::Replay) {
	game.showPressEnterText=false;
	game.startReplay();
}
GPReplay::~GPReplay() {}

void GPReplay::update() {
	if (game.playReplay())
		Signals::SetGameState(GameStates::GameOver);
}

//////////////////////////////////////////////////////////////////////
//							Practice								//
//////////////////////////////////////////////////////////////////////

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

//////////////////////////////////////////////////////////////////////
//							Spectating								//
//////////////////////////////////////////////////////////////////////

GPSpectating::GPSpectating(gamePlay& _game) : GPBaseState(_game, GameStates::Spectating) {}
GPSpectating::~GPSpectating() {}