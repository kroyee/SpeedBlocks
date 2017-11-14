#ifndef GAMEDRAW_H
#define GAMEDRAW_H

#include <SFML/System.hpp>
#include <thread>
#include <atomic>

class Resources;
class GuiElements;

class GameDraw {
	sf::Time nextDraw, nextUpdate;
	Resources& resources;
	GuiElements& guiElements;
	bool& drawMe;

	std::thread drawThread;
	std::atomic<uint8_t> status, fieldsDone, fieldsTotal;

	void drawThreadLoop();

public:
	GameDraw(Resources& _res, GuiElements& _elems, bool& _drawMe);
	void draw();
	void init();
	void quit();
};

#endif