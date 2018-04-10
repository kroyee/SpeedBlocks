#ifndef GAMEFIELDDRAWER_H
#define GAMEFIELDDRAWER_H

#include <list>
#include <mutex>
#include <unordered_set>
#include <deque>
#include "gameField.h"

class Resources;

class GameFieldDrawer {
	std::list<obsField>::iterator queueIt;
public:
	GameFieldDrawer(Resources&);

	Resources& resources;

	std::mutex fieldsMutex;
	std::list<obsField> fields;
	std::list<obsField> unusedFields;

	sf::Clock sclock;
	obsField* scaleup;

	float currentR;

	short xPos, yPos, width, height;

	bool visible;

	void show() { visible=true; }
	void hide() { visible=false; }

	bool isVisible() { return visible; }

	void setPosition(short x, short y);
	void setSize(int w, int h);

	obsField& addField(int id, const std::string& name);
	void removeField(int id);
	void updateFields();
	void calFieldPos();

	void removeAllFields();

	void resetOppFields();

	void drawOppField(obsField& field);
	void drawFields();
	bool drawNextField();
	void drawScaleup();

	void enlargePlayfield(sf::Event& event);

	void setBackgroundColor(int val);
	int areThereFields(int type);
};

#endif