#include "GameplayGameState.h"
#include "gamePlay.h"

GPBaseState::GPBaseState(gamePlay& _game, GameStates _state) : state(_state), game(_game) {
	game.resources.gamestate = state;
}
GPBaseState::~GPBaseState() {}

void GPBaseState::handleGeneralButtons(sf::Event& event) { // != Replay && MainMenu
	if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == game.options.score)
            Signals::Show(13);
        else if (event.key.code == game.options.away && game.resources.playonline)
            Signals::Away();
	}
	else if (event.type == sf::Event::KeyReleased)
		if (event.key.code == game.options.score)
			Signals::Hide(13);
}

void GPBaseState::handleControlButtons(sf::Event& event) { // == Game || Practice
	if (event.type == sf::Event::KeyPressed && !game.resources.chatFocused) {
        if (event.key.code == game.options.right)
            game.mRKey();
        else if (event.key.code == game.options.left)
            game.mLKey();
        else if (event.key.code == game.options.rcw)
            game.rcw();
        else if (event.key.code == game.options.rccw)
            game.rccw();
        else if (event.key.code == game.options.r180)
            game.r180();
        else if (event.key.code == game.options.down)
            game.mDKey();
        else if (event.key.code == game.options.hd)
            game.hd();
    }
    else if (event.type == sf::Event::KeyReleased) {
        if (event.key.code == game.options.right)
            game.sRKey();
        else if (event.key.code == game.options.left)
            game.sLKey();
        else if (event.key.code == game.options.down)
            game.sDKey();
    }
}

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
    game.field.text.ready=false;
}
GPCountDown::~GPCountDown() {}

void GPCountDown::update() {
	if (!game.resources.playonline)
        if (game.countDown())
            Signals::SetGameState(GameStates::Game);
}

void GPCountDown::handleEvent(sf::Event& event) {
	handleGeneralButtons(event);

	if (event.type == sf::Event::KeyPressed && !game.resources.chatFocused) {
        if (event.key.code == game.options.right)
            game.rKey=true;
        else if (event.key.code == game.options.left)
            game.lKey=true;
    }
    else if (event.type == sf::Event::KeyReleased) {
        if (event.key.code == game.options.right)
            game.rKey=false;
        else if (event.key.code == game.options.left)
            game.lKey=false;
    }
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

void GPGame::handleEvent(sf::Event& event) {
	handleGeneralButtons(event);
	handleControlButtons(event);
}

//////////////////////////////////////////////////////////////////////
//							Game Over								//
//////////////////////////////////////////////////////////////////////

GPGameOver::GPGameOver(gamePlay& _game) : GPBaseState(_game, GameStates::GameOver) {
	if (game.autoaway && game.resources.playonline)
    	Signals::SetAway(true);
    game.field.text.setCountdown(0);
    game.drawMe=true;
}
GPGameOver::~GPGameOver() {}

void GPGameOver::handleEvent(sf::Event& event) {
	handleGeneralButtons(event);

	if (event.type == sf::Event::KeyPressed && !game.resources.chatFocused) {
        if (event.key.code == sf::Keyboard::P && !Signals::IsVisible(5)) {
        	if (game.resources.playonline) {
        		if (Signals::IsVisible(8))
        			Signals::Ready();
        		else
        			Signals::SetGameState(GameStates::Practice);
        	}
        	else {
                Signals::SetGameState(GameStates::CountDown);
                game.startCountdown();
        	}
        }
        else if (event.key.code == game.options.ready && game.resources.playonline)
        	Signals::Ready();
    }
}

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
	Signals::SendSig(7);
	game.field.clear();
	game.showPressEnterText=false;
    game.startGame();
}
GPPractice::~GPPractice() {
	game.autoaway = false;
	game.field.text.away=false;
	game.field.text.ready=true;
}

void GPPractice::update() {
	game.delayCheck();
}

void GPPractice::handleEvent(sf::Event& event) {
	handleGeneralButtons(event);
	handleControlButtons(event);
}

//////////////////////////////////////////////////////////////////////
//							Spectating								//
//////////////////////////////////////////////////////////////////////

GPSpectating::GPSpectating(gamePlay& _game) : GPBaseState(_game, GameStates::Spectating) {}
GPSpectating::~GPSpectating() {}