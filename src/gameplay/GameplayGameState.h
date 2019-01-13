#ifndef GAMEPLAYGAMESTATE_H
#define GAMEPLAYGAMESTATE_H

#include "Resources.h"

class GamePlay;

class GPBaseState {
public:
	const GameStates state;

	GPBaseState(GamePlay& _game, GameStates _state);
	virtual ~GPBaseState();

	virtual void update() {}
	virtual void handleEvent(sf::Event&) {}

	void handleGeneralButtons(sf::Event& event);
	void handleControlButtons(sf::Event& event);

	static void set(std::unique_ptr<GPBaseState>& state, GameStates _state);

protected:
	GamePlay& game;
};

class GPMainMenu : public GPBaseState {
public:
	GPMainMenu(GamePlay& _game);
	~GPMainMenu();
};

class GPCountDown : public GPBaseState {
public:
	GPCountDown(GamePlay& _game);
	~GPCountDown();

	void update() override;
	void handleEvent(sf::Event& event) override;
};

class GPGame : public GPBaseState {
public:
	GPGame(GamePlay& _game);
	~GPGame();

	void update() override;
	void handleEvent(sf::Event& event) override;
};

class GPGameOver : public GPBaseState {
public:
	GPGameOver(GamePlay& _game);
	~GPGameOver();

	void handleEvent(sf::Event& event) override;
};

class GPReplay : public GPBaseState {
public:
	GPReplay(GamePlay& _game);
	~GPReplay();

	void update() override;
};

class GPPractice : public GPBaseState {
public:
	GPPractice(GamePlay& _game);
	~GPPractice();

	void update() override;
	void handleEvent(sf::Event& event) override;
};

class GPSpectating : public GPBaseState {
public:
	GPSpectating(GamePlay& _game);
	~GPSpectating();
};

#endif