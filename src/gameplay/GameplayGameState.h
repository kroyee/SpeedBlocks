#ifndef GAMEPLAYGAMESTATE_H
#define GAMEPLAYGAMESTATE_H

#include "Resources.h"

class gamePlay;

class GPBaseState {
public:
	GPBaseState(gamePlay& _game, GameStates _state);
	virtual ~GPBaseState();
	gamePlay& game;
	const GameStates state;

	static void set(std::unique_ptr<GPBaseState>& state, GameStates _state);
};

class GPMainMenu : public GPBaseState {
public:
	GPMainMenu(gamePlay& _game);
	~GPMainMenu();
};

class GPCountDown : public GPBaseState {
public:
	GPCountDown(gamePlay& _game);
	~GPCountDown();
};

class GPGame : public GPBaseState {
public:
	GPGame(gamePlay& _game);
	~GPGame();
};

class GPGameOver : public GPBaseState {
public:
	GPGameOver(gamePlay& _game);
	~GPGameOver();
};

class GPReplay : public GPBaseState {
public:
	GPReplay(gamePlay& _game);
	~GPReplay();
};

class GPPractice : public GPBaseState {
public:
	GPPractice(gamePlay& _game);
	~GPPractice();
};

class GPSpectating : public GPBaseState {
public:
	GPSpectating(gamePlay& _game);
	~GPSpectating();
};

#endif