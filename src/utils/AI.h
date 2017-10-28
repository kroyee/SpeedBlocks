#ifndef AI_H
#define AI_H

#include "gameField.h"
#include "randomizer.h"
#include "TestField.h"
#include <deque>

class Resources;

enum class Mode { Downstack, Stack };

class AI {
public:
	Resources& resources;
	std::array<double, 10> weights, downstackWeights, stackWeights;
	obsField field;
	TestField firstMove, secondMove;

	uint8_t nextpiece;
	int32_t score, density_score;

	sf::Vector2i well2Pos;

	uint16_t garbageCleared;
	uint16_t garbageAdded;
	uint16_t linesCleared;
	uint16_t moveCount;
	uint16_t gameCount;

	Mode mode;
	bool training;

	AI(Resources& _res);
	
	void randomizeWeights(randomizer& rand);

	bool makeMove(MoveInfo& move, int nextpiece);
	void setPiece(int piece);

	void calcWell();
	void checkFieldDensity();

	void restartAI();
	void restartGame();
	void addGarbageLine(uint8_t);

	void setMode(Mode);

	void draw();

	void stepping();
};

struct AIResults {
	AIResults() {}

	AIResults(std::array<double, 10> & _weights, uint32_t garb, uint32_t lines) :
	weights(_weights),
	garbageCleared(garb),
	linesCleared(lines),
	score(garb*2 + lines) {}

	void printInfo();
	std::array<double, 10> weights;
	uint32_t garbageCleared, linesCleared, score;
};

class Population {
public:
	Resources& resources;
	randomizer rander;
	std::deque<AIResults> results;
	std::deque<AIResults> offspring;
	AI ai;
	uint16_t cycleCount;
	bool quit=false;

	Population(Resources& res);
	void runAI(AIResults& bot, int count);
	void showAI(AIResults& result);
	void showBest();
	bool populationCycle(int games);

	AIResults tournamentSelection();
	void mutate(AIResults&);
	void insertOffspring();
	void evolvePopulation();

	bool handleEvents();

	void makePopulation(int size);
	void savePopulation();
	bool loadPopulation(const std::string & filename);
};

#endif