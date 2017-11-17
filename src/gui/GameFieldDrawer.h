#ifndef GAMEFIELDDRAWER_H
#define GAMEFIELDDRAWER_H

#include <list>
#include <mutex>
#include "gameField.h"

class Resources;

class GameFieldDrawer {
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

	obsField& addField(int id, const sf::String& name);
	void removeField(int id);
	void updateField(obsField& field);
	void calFieldPos();

	void removeAllFields();

	void resetOppFields();

	void drawOppField(obsField& field);
	void drawFields();

	void enlargePlayfield(sf::Event& event);

	void setBackgroundColor(int val);
	int areThereFields(int type);
};

#endif