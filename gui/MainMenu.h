#ifndef MAINMENU_H
#define MAINMENU_H

#include "guiBase.h"

class UI;

class Menu : public guiBase {
public:
	void create(sf::Rect<int> _pos, UI* _ui);
	void playOnline();
	void Training();
	void Options();
	void quitGame();
};

#endif