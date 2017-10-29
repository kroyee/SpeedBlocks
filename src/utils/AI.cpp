#include "AI.h"
#include "optionSet.h"
#include "Resources.h"
#include <fstream>
#include <iostream>
using std::cout;
using std::endl;

const sf::Time finesseTime = sf::milliseconds(20);
const sf::Time moveTime = sf::milliseconds(50);
const bool useStepping = false;

AI::AI(Resources& _res) : resources(_res), field(_res), firstMove(_res), secondMove(_res) {}

void AI::randomizeWeights(randomizer& rander) {
	for (int i=0; i<10; i++)
		weights[i] = rander.piece_dist(rander.piece_gen) * 2 - 1;

/*	if (mode == Mode::Downstack)
		return;

	weights[0] = 0;		// totalHeiht
	weights[1] = -0.7;	// closedHoles
	weights[2] = -0.1;	// bumpiness
	weights[3] = -0.1;	// totalLines
	weights[4] = 0.1;	// well2wide
	weights[5] = -0.1;	// highestpoint
	weights[6] = 0;		// ontopofholes
	weights[7] = -0.2;	// well1wide
	weights[8] = 0;		// pieceNextToWall
	weights[9] = -0.35;	// openHoles

	for (int i=0; i<10; i++)
		weights[i] += rander.piece_dist(rander.piece_gen) * 0.4 - 0.2;*/
}

bool AI::makeMove(MoveInfo& move, int _nextpiece) {
	field.piece.posX = move.posX;
	field.piece.posY = 0;
	while (field.piece.current_rotation != move.rot)
		field.piece.rcw();

	if (move.use_path) {
		for (auto it = move.path.rbegin(); it != move.path.rend(); it++) {
			if (!training) {
				draw();
				if (useStepping)
					stepping();
				sf::sleep(finesseTime);
			}

			if (*it == 255)
				field.mRight();
			else if (*it == 254)
				field.mLeft();
			else if (*it == 253)
				field.hd();
			else if (*it == 241)
				field.rcw();
			else if (*it == 242)
				field.r180();
			else if (*it == 243)
				field.rccw();
			else
				for (int i = 0; i < *it; i++)
					field.mDown();
		}
		if (!training && useStepping)
			stepping();
	}
	field.hd();
	field.addPiece();

	auto lines = field.clearlines();
	garbageCleared += lines.y;
	linesCleared += lines.x;
	moveCount++;

	setPiece(_nextpiece);

	if (!field.possible())
		return true;
	return false;
}

void AI::setPiece(int piece) {
	field.piece.piece = piece;
	field.piece.tile = resources.options->basepiece[piece].tile;
	field.piece.rotation = resources.options->basepiece[piece].rotation;
	field.piece.posX = 3;
	field.piece.posY = 0;

	field.updatePiece();
}

void AI::calcWell() {
	int lowestPieceAmount=44;
	for (int x=0; x<9; x++) {
		int pieceAmount=0;
		for (int y=0; y<22; y++) {
			if (field.square[y][x])
				pieceAmount++;
			if (field.square[y][x+1])
				pieceAmount++;
		}
		if (pieceAmount < lowestPieceAmount) {
			lowestPieceAmount = pieceAmount;
			well2Pos.x=x;
			well2Pos.y=x+1;
		}
	}
}

void AI::checkFieldDensity() {
	calcWell();
	int full, empty;
	density_score=0;
	for (int x=0; x<10; x++) {
		full=0; empty=0;
		for (int y=0; y<22; y++) {
			if (field.square[y][x])
				full++;
			else
				empty++;
		}

		if (x == well2Pos.x || x == well2Pos.y)
			density_score += empty*10;
		else
			density_score += full*10;
	}
	density_score -= moveCount*3;
}

void AI::restartAI() {
	moveCount=0;
	garbageCleared=0;
	linesCleared=0;
	gameCount=0;
	garbageAdded=0;
	density_score=0;
	field.clear();
}

void AI::restartGame() {
	if (mode == Mode::Downstack)
		garbageAdded=garbageCleared;
	else if (mode == Mode::Stack)
		checkFieldDensity();
	auto pieceBackup = field.piece.piece;
	field.clear();
	field.piece.piece = pieceBackup;
	gameCount++;
	moveCount=0;
}

void AI::addGarbageLine(uint8_t hole) {
	for (int y=0; y<21; y++)
		for (int x=0; x<10; x++)
			field.square[y][x]=field.square[y+1][x];
	for (int x=0; x<10; x++)
		field.square[21][x]=8;
	field.square[21][hole]=0;
	garbageAdded++;
}

void AI::setMode(Mode _mode) {
	mode = _mode;
	if (mode == Mode::Downstack) {
		cout << "Downstack" << endl;
		weights[0] = -0.777562;
		weights[1] = -0.957217;
		weights[2] = -0.206355;
		weights[3] = 0.305608;
		weights[4] = -0.0985396;
		weights[5] = -0.571009;
		weights[6] = -0.0826352;
		weights[7] = -0.268683;
		//weights[8] = ?
		//weights[9] = ?
		firstMove.weights = weights;
		secondMove.weights = weights;
	}
	else if (mode == Mode::Stack) {
		cout << "Stack" << endl;
		weights[0] = 0.0646257;
		weights[1] = -0.781367;
		weights[2] = -0.079562;
		weights[3] = -0.112896;
		weights[4] = 0.238397;
		weights[5] = -0.136575;
		weights[6] = -0.0488756;
		weights[7] = -0.206737;
		//weights[8] = ?
		//weights[9] = ?
		firstMove.weights = weights;
		secondMove.weights = weights;
	}
}

void AI::draw() {
	resources.window.clear(sf::Color(50,50,50));
	field.drawField();
	resources.window.draw(field.sprite);
	resources.window.display();
}

void AI::stepping() {
	bool wait=true;
	sf::Event event;
	while (wait) {
		resources.window.waitEvent(event);
		if (event.type == sf::Event::KeyPressed)
			wait=false;
		draw();
	}
}

void AIResults::printInfo() {
	cout << "totalHeight " << weights[0] << "\nclosedHoles " << weights[1] << "\nbumbiness " << weights[2];
	cout << "\ntotalLines " << weights[3] << "\nwell2Wide " << weights[4] << "\nhighestPoint " << weights[5];
	cout << "\nonTopOfHoles " << weights[6] << "\nwell1Wide " << weights[7] << "\nnextToWall " << weights[8];
	cout << "\nopenHoles " << weights[9] << "\nTotal score " << score << endl;
}

/////////////////////////////////////////
///////////// Population ////////////////
/////////////////////////////////////////

Population::Population(Resources& res) : resources(res), ai(res), quit(false) {
	rander.seedPiece(time(NULL));
}

void Population::runAI(AIResults& bot, int count) {
	ai.restartAI();
	ai.setPiece(rander.getPiece());
	ai.weights = bot.weights;
	ai.firstMove.weights = ai.weights;
	ai.secondMove.weights = ai.weights;
	ai.training = true;

	while (ai.gameCount < count) {
		if (ai.mode == Mode::Downstack)
			while (ai.garbageAdded - ai.garbageCleared < 5)
				ai.addGarbageLine(rander.getHole());

		ai.firstMove.square = ai.field.square;
		ai.firstMove.setPiece(ai.field.piece.piece);
		ai.firstMove.calcHolesBeforePiece();
		ai.firstMove.findBestMove();
		if (ai.makeMove(ai.firstMove.move, rander.getPiece()))
			ai.restartGame();

		if (ai.mode == Mode::Downstack) {
			if (ai.moveCount > 500)
				ai.restartGame();
		}
		else if (ai.mode == Mode::Stack) {
			if (ai.moveCount > 80 || ai.firstMove.totalHeight > 128)
				ai.restartGame();
		}

		if (handleEvents())
			break;
	}

	bot.garbageCleared = ai.garbageCleared;
	bot.linesCleared = ai.linesCleared;
	if (ai.mode == Mode::Downstack)
		bot.score = bot.garbageCleared*2 + bot.linesCleared;
	else if (ai.mode == Mode::Stack)
		bot.score = ai.density_score;
}

void Population::showAI(AIResults& result) {
	ai.restartAI();
	ai.weights = result.weights;
	ai.firstMove.weights = ai.weights;
	ai.secondMove.weights = ai.weights;
	ai.setPiece(rander.getPiece());
	ai.nextpiece = rander.getPiece();
	ai.training = false;

	result.printInfo();

	while (ai.gameCount < 1) {
		if (ai.mode == Mode::Downstack)
			while (ai.garbageAdded - ai.garbageCleared < 5)
				ai.addGarbageLine(rander.getHole());
		
		ai.draw();

		if (handleEvents())
			break;

		ai.firstMove.square = ai.field.square;
		ai.firstMove.setPiece(ai.field.piece.piece);
		ai.firstMove.calcHolesBeforePiece();
		ai.firstMove.tryAllMoves(ai.secondMove, ai.nextpiece);
		//ai.firstMove.findBestMove();
		if (ai.makeMove(ai.firstMove.move, ai.nextpiece))
			ai.restartGame();
		ai.nextpiece = rander.getPiece();

		/*ai.firstMove.calcHeightsAndHoles();
		if (ai.firstMove.totalHeight > 130)
			ai.setMode(Mode::Downstack);
		else if (ai.firstMove.totalHeight < 10)
			ai.setMode(Mode::Stack);*/

		/*auto hbpBackup = ai.firstMove.closedHoles;
		auto bu2 = ai.firstMove.openHoles;
		ai.firstMove.square = ai.field.square;
		ai.firstMove.calcHolesBeforePiece();
		if (ai.firstMove.closedHoles > hbpBackup) {
			cout << (int)ai.firstMove.closedHoles << " " << (int)hbpBackup << " - ";
			cout << (int)ai.firstMove.openHoles << " " << (int)bu2 << endl;
			ai.draw();
			ai.stepping();
		}*/

		/*if (ai.mode == Mode::Downstack) {
			if (ai.moveCount > 500)
				ai.restartGame();
		}
		else if (ai.mode == Mode::Stack) {
			if (ai.moveCount > 60 || ai.totalHeight > 160)
				ai.restartGame();
		}*/

		sf::sleep(moveTime);
	}
}

void Population::showBest() {
	uint32_t index=0, highest=0;
	for (unsigned int i=0; i<results.size(); i++) {
		if (results[i].score > highest) {
			highest = results[i].score;
			index=i;
		}
	}

	showAI(results[index]);
}

bool Population::populationCycle(int games) {
	int count=0;
	for (auto& bot : results) {
		count++;
		runAI(bot, games);
		if (count % 10 == 0)
			cout << count << " of " << results.size() << endl;
		if (quit)
			return false;
	}
	return true;
}

AIResults Population::tournamentSelection() {
	int first=-1, second=-2;
	uint32_t firstScore=0, secondScore=0;
	int size = results.size();
	for (int i=0; i<size/10.0; i++) {
		int index = rander.piece_dist(rander.piece_gen)*size;
		if (results[index].score > firstScore && index != first) {
			second=first;
			secondScore=firstScore;

			firstScore = results[index].score;
			first = index;
		}

		if (index != first && index != second && results[index].score > secondScore) {
			secondScore = results[index].score;
			second = index;
		}
	}

	AIResults offspring;
	double combinationRatio = (results[first].score - results[second].score) / static_cast<double>(results[first].score);
	for (int i=0; i<10; i++) {
		double diff = (results[first].weights[i] - results[second].weights[i]) / 2.0;
		offspring.weights[i] = results[second].weights[i] + diff + diff*combinationRatio;
	}

	return offspring;
}

void Population::mutate(AIResults& result) {
	if (rander.piece_dist(rander.piece_gen) > 0.05)
		return;

	int index = rander.piece_dist(rander.piece_gen)*10;
	double add = rander.piece_dist(rander.piece_gen)*0.4-0.2;
	double normal = 1 + (add < 0 ? add*-1 : add);
	result.weights[index] += add;
	result.weights[index] /= normal;
}

void Population::insertOffspring() {
	std::sort(results.begin(), results.end(), [](AIResults& first, AIResults& second){ return first.score < second.score; });
	while (!offspring.empty()) {
		results.pop_front();
		results.push_back(offspring.front());
		offspring.pop_front();
	}
}

void Population::evolvePopulation() {
	offspring.clear();
	while (offspring.size() < results.size() * 0.3) {
		offspring.push_back(tournamentSelection());
		mutate(offspring.back());
	}
	insertOffspring();
}

bool Population::handleEvents() {
	sf::Event event;

    while (resources.window.pollEvent(event)) {
    	if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::C && event.key.control) {
    		cout << "Aborting..." << endl;
    		quit=true;
    		return true;
    	}
    }

    return false;
}

void Population::savePopulation() {
	char modeByte = (char)255;
	std::string filename = "AI/population." + std::to_string(cycleCount) + "-cycles.";
	if (ai.mode == Mode::Downstack) {
		filename += "downstack";
		modeByte=0;
	}
	else if (ai.mode == Mode::Stack) {
		filename += "stack";
		modeByte=1;
	}
	std::ofstream file(filename, std::ios::binary);
	size_t size = sizeof(double);

	if (!file.is_open()) {
		std::cout << "Error saving population" << std::endl;
		return;
	}

	file.write(reinterpret_cast<char*>(&cycleCount), sizeof(uint16_t));
	file.write(&modeByte, 1);

	for (auto& bot : results) {
		file.write(reinterpret_cast<char*>(&bot.weights[0]), size*10);
		file.write(reinterpret_cast<char*>(&bot.score), sizeof(uint32_t));
	}

	file.close();

	cout << "Population saved" << endl;
}

void Population::makePopulation(int size) {
	results.clear();
	for (int i=0; i<size; i++) {
		ai.randomizeWeights(rander);
		results.push_back(AIResults(ai.weights, 0, 0));
	}
	cycleCount=1;
}

bool Population::loadPopulation(const std::string & filename) {
	results.clear();
	std::ifstream file(filename, std::ios::binary);
	size_t size = sizeof(double);

	if (!file.is_open()) {
		std::cout << "Error loading population" << std::endl;
		return false;
	}

	file.read(reinterpret_cast<char*>(&cycleCount), sizeof(uint16_t));
	char modeByte;
	file.read(&modeByte, 1);
	if (modeByte == 0)
		ai.mode = Mode::Downstack;
	else if (modeByte == 1)
		ai.mode = Mode::Stack;
	else {
		cout << "Could not determine what mode the population has!!!!" << endl;
		file.close();
		return false;
	}

	AIResults bot;
	while (file.read(reinterpret_cast<char*>(&bot.weights[0]), size*10) && file.read(reinterpret_cast<char*>(&bot.score), sizeof(uint32_t)))
		results.push_back(bot);

	cout << "Loaded population of " << results.size() << " bots with cycle count " << cycleCount << endl;
	file.close();

	return true;
}