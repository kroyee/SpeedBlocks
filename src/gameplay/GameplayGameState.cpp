#include "GameplayGameState.h"
#include "GamePlay.h"

static auto& Show = Signal<void, int>::get("Show");
static auto& Away = Signal<void>::get("Away");
static auto& Hide = Signal<void, int>::get("Hide");
static auto& AmountAI = Signal<void, uint8_t>::get("AmountAI");
static auto& SetGameState = Signal<void, GameStates>::get("SetGameState");
static auto& SetAway = Signal<void, bool>::get("SetAway");
static auto& IsVisible = Signal<bool, int>::get("IsVisible");
static auto& Ready = Signal<void>::get("Ready");

GPBaseState::GPBaseState(GamePlay& _game, GameStates _state) : state(_state), game(_game) {
	game.resources.gamestate = state;
}
GPBaseState::~GPBaseState() {}

void GPBaseState::handleGeneralButtons(sf::Event& event) { // != Replay && MainMenu
	static auto& score = Options::get<sf::Keyboard::Key>("score");
	static auto& away = Options::get<sf::Keyboard::Key>("away");
	if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == score)
            Show(13);
        else if (event.key.code == away && game.resources.playonline)
            Away();
	}
	else if (event.type == sf::Event::KeyReleased)
		if (event.key.code == score)
			Hide(13);
}

void GPBaseState::handleControlButtons(sf::Event& event) { // == Game || Practice
	static auto& right = Options::get<sf::Keyboard::Key>("right");
	static auto& left = Options::get<sf::Keyboard::Key>("left");
	static auto& rcw = Options::get<sf::Keyboard::Key>("rcw");
	static auto& rccw = Options::get<sf::Keyboard::Key>("rccw");
	static auto& r180 = Options::get<sf::Keyboard::Key>("r180");
	static auto& down = Options::get<sf::Keyboard::Key>("down");
	static auto& hd = Options::get<sf::Keyboard::Key>("hd");

	if (event.type == sf::Event::KeyPressed && !game.resources.chatFocused) {
        if (event.key.code == right)
            game.mRKey();
        else if (event.key.code == left)
            game.mLKey();
        else if (event.key.code == rcw)
            game.rcw();
        else if (event.key.code == rccw)
            game.rccw();
        else if (event.key.code == r180)
            game.r180();
        else if (event.key.code == down)
            game.mDKey();
        else if (event.key.code == hd)
            game.hd();
    }
    else if (event.type == sf::Event::KeyReleased) {
        if (event.key.code == right)
            game.sRKey();
        else if (event.key.code == left)
            game.sLKey();
        else if (event.key.code == down)
            game.sDKey();
    }
}

void GPBaseState::set(std::unique_ptr<GPBaseState>& state, GameStates _state) {
	GamePlay& gpRef = state->game;
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

GPMainMenu::GPMainMenu(GamePlay& _game) : GPBaseState(_game, GameStates::MainMenu) {
	game.field.clear();
	game.autoaway=false;
	game.field.text.hide<FieldText::Away>();
	AmountAI(0);
}
GPMainMenu::~GPMainMenu() {
	game.showPressEnterText=true;
}

//////////////////////////////////////////////////////////////////////
//							Countdown								//
//////////////////////////////////////////////////////////////////////

GPCountDown::GPCountDown(GamePlay& _game) : GPBaseState(_game, GameStates::CountDown) {
	game.sRKey();
    game.sLKey();
    game.sDKey();
    game.showPressEnterText=false;
    game.drawMe=true;
    game.field.text.hide<FieldText::Ready>();
}
GPCountDown::~GPCountDown() {}

void GPCountDown::update() {
	if (!game.resources.playonline)
        if (game.countDown())
            SetGameState(GameStates::Game);
}

void GPCountDown::handleEvent(sf::Event& event) {
	handleGeneralButtons(event);

	static auto& left = Options::get<sf::Keyboard::Key>("left");
	static auto& right = Options::get<sf::Keyboard::Key>("right");

	if (event.type == sf::Event::KeyPressed && !game.resources.chatFocused) {
        if (event.key.code == right)
            game.rKey=true;
        else if (event.key.code == left)
            game.lKey=true;
    }
    else if (event.type == sf::Event::KeyReleased) {
        if (event.key.code == right)
            game.rKey=false;
        else if (event.key.code == left)
            game.lKey=false;
    }
}

//////////////////////////////////////////////////////////////////////
//							Game									//
//////////////////////////////////////////////////////////////////////

GPGame::GPGame(GamePlay& _game) : GPBaseState(_game, GameStates::Game) {
	game.showPressEnterText=false;
    game.startGame();
	game.field.text.hide<FieldText::Countdown>();
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

GPGameOver::GPGameOver(GamePlay& _game) : GPBaseState(_game, GameStates::GameOver) {
	if (game.autoaway && game.resources.playonline)
    	SetAway(true);
    game.field.text.hide<FieldText::Countdown>();
    game.drawMe=true;
}
GPGameOver::~GPGameOver() {}

void GPGameOver::handleEvent(sf::Event& event) {
	handleGeneralButtons(event);
	static auto& ready = Options::get<sf::Keyboard::Key>("ready");

	if (event.type == sf::Event::KeyPressed && !game.resources.chatFocused) {
        if (event.key.code == sf::Keyboard::P && !IsVisible(5)) {
        	if (game.resources.playonline) {
        		if (IsVisible(8))
        			Ready();
        		else
        			SetGameState(GameStates::Practice);
        	}
        	else {
                SetGameState(GameStates::CountDown);
                game.startCountdown();
        	}
        }
        else if (event.key.code == ready && game.resources.playonline)
        	Ready();
    }
}

//////////////////////////////////////////////////////////////////////
//							Replay									//
//////////////////////////////////////////////////////////////////////

GPReplay::GPReplay(GamePlay& _game) : GPBaseState(_game, GameStates::Replay) {
	game.showPressEnterText=false;
	game.startReplay();
}
GPReplay::~GPReplay() {}

void GPReplay::update() {
	if (game.playReplay())
		SetGameState(GameStates::GameOver);
}

//////////////////////////////////////////////////////////////////////
//							Practice								//
//////////////////////////////////////////////////////////////////////

GPPractice::GPPractice(GamePlay& _game) : GPBaseState(_game, GameStates::Practice) {
	SendSignal(7);
	game.field.clear();
	game.showPressEnterText=false;
    game.startGame();
}
GPPractice::~GPPractice() {
	game.autoaway = false;
	game.field.text.hide<FieldText::Away>();
	game.field.text.show<FieldText::Ready>();
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

GPSpectating::GPSpectating(GamePlay& _game) : GPBaseState(_game, GameStates::Spectating) {}
GPSpectating::~GPSpectating() {}
