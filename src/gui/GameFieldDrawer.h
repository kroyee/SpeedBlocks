#ifndef GAMEFIELDDRAWER_H
#define GAMEFIELDDRAWER_H

#include <list>
#include "gameField.h"

class Resources;

class GameFieldDrawer {
public:
	GameFieldDrawer(Resources&);

	Resources& resources;

	std::list<obsField> fields;

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

	void addField(obsField& field);
	void removeField(sf::Uint16 id);
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