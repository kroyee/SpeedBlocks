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
	virtual void handleEvent(sf::Event&) {}

	void handleGeneralButtons(sf::Event& event);
	void handleControlButtons(sf::Event& event);

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
	void handleEvent(sf::Event& event) override;
};

class GPGame : public GPBaseState {
public:
	GPGame(gamePlay& _game);
	~GPGame();

	void update() override;
	void handleEvent(sf::Event& event) override;
};

class GPGameOver : public GPBaseState {
public:
	GPGameOver(gamePlay& _game);
	~GPGameOver();

	void handleEvent(sf::Event& event) override;
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

	void update() override;
	void handleEvent(sf::Event& event) override;
};

class GPSpectating : public GPBaseState {
public:
	GPSpectating(gamePlay& _game);
	~GPSpectating();
};

#endif