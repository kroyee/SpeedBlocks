#include "UIGameState.h"
#include "gui.h"
#include "GuiElements.h"
#include "gamePlay.h"

UIBaseState::UIBaseState(UI& _ui, GameStates _state) : ui(_ui), state(_state) {
	ui.gamestate = state;
}
UIBaseState::~UIBaseState() {}

UIMainMenu::UIMainMenu(UI& _ui) : UIBaseState(_ui, GameStates::MainMenu) {
	ui.guiElements->gameFieldDrawer.removeAllFields();
	ui.game.field.clear();
	ui.away=false;
	ui.game.autoaway=false;
	ui.game.field.text.away=false;
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
		ui.guiElements->mainMenu.show();
}
UIMainMenu::~UIMainMenu() {
	ui.guiElements->mainMenu.hide();
	ui.guiElements->onlineplayUI.hide();
	ui.guiElements->gameFieldDrawer.show();
	ui.guiElements->gameStandings.hide();
	ui.game.showPressEnterText=true;
}


UICountDown::UICountDown(UI& _ui) : UIBaseState(_ui, GameStates::CountDown) {
	ui.game.sRKey();
    ui.game.sLKey();
    ui.game.sDKey();
    ui.game.showPressEnterText=false;
    ui.game.draw();
}
UICountDown::~UICountDown() {

}


UIGame::UIGame(UI& _ui) : UIBaseState(_ui, GameStates::Game) {
	ui.linesSent=0;
    ui.garbageCleared=0;
    ui.linesBlocked=0;
	ui.game.showPressEnterText=false;
    ui.game.startGame();
}
UIGame::~UIGame() {
	ui.game.showPressEnterText=true;
}


UIGameOver::UIGameOver(UI& _ui) : UIBaseState(_ui, GameStates::GameOver) {
	if (ui.game.autoaway)
    	Signals::SetAway(true);
    ui.game.field.text.setCountdown(0);
    ui.game.draw();
}
UIGameOver::~UIGameOver() {

}


UIReplay::UIReplay(UI& _ui) : UIBaseState(_ui, GameStates::Replay) {
	ui.game.showPressEnterText=false;
	ui.game.startReplay();
}
UIReplay::~UIReplay() {
	ui.guiElements->replayUI.playPause->setText("Play");
	ui.guiElements->replayUI.pauseTime=sf::seconds(0);
}


UIPractice::UIPractice(UI& _ui) : UIBaseState(_ui, GameStates::Practice) {
	ui.linesSent=0;
    ui.garbageCleared=0;
    ui.linesBlocked=0;
	if (!ui.game.field.text.ready)
		Signals::Ready();
	if (ui.away)
		Signals::SetAway(false);
	ui.game.field.clear();
	ui.game.showPressEnterText=false;
    ui.game.startGame();
}
UIPractice::~UIPractice() {
	ui.game.autoaway = false;
	ui.game.field.text.away=false;
	ui.game.field.text.ready=true;
}


UISpectating::UISpectating(UI& _ui) : UIBaseState(_ui, GameStates::Spectating) {}
UISpectating::~UISpectating() {
	Signals::SendSig(20);
}