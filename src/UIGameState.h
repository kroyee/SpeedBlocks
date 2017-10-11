#ifndef UIGAMESTATE_H
#define UIGAMESTATE_H

#include "Resources.h"

class UI;

class UIBaseState {
public:
	UIBaseState(UI& _ui, GameStates _state);
	virtual ~UIBaseState();
	UI& ui;
	const GameStates state;
};

class UIMainMenu : public UIBaseState {
public:
	UIMainMenu(UI& _ui);
	~UIMainMenu();
};

class UICountDown : public UIBaseState {
public:
	UICountDown(UI& _ui);
	~UICountDown();
};

class UIGame : public UIBaseState {
public:
	UIGame(UI& _ui);
	~UIGame();
};

class UIGameOver : public UIBaseState {
public:
	UIGameOver(UI& _ui);
	~UIGameOver();
};

class UIReplay : public UIBaseState {
public:
	UIReplay(UI& _ui);
	~UIReplay();
};

class UIPractice : public UIBaseState {
public:
	UIPractice(UI& _ui);
	~UIPractice();
};

class UISpectating : public UIBaseState {
public:
	UISpectating(UI& _ui);
	~UISpectating();
};

#endif