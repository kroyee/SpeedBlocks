#ifndef GAMEPLAYGAMESTATE_H
#define GAMEPLAYGAMESTATE_H

#include "Resources.h"

class gamePlay;

class GPBaseState {
public:
	const GameStates state;

	GPBaseState(gamePlay& _game, GameStates _state);
	virtual ~GPBaseState();

	virtual void update() {}

	static void set(std::unique_ptr<GPBaseState>& state, GameStates _state);

protected:
	gamePlay& game;
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

	void update() override;
};

class GPGame : public GPBaseState {
public:
	GPGame(gamePlay& _game);
	~GPGame();

	void update() override;
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

	void update() override;
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