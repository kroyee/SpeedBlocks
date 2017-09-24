#ifndef MAINMENU_H
#define MAINMENU_H

#include "guiBase.h"

class Resources;

class Menu : public guiBase {
public:
	Menu(sf::Rect<int> _pos, Resources& _res);
	void Training();
	void quitGame();
};

#endif