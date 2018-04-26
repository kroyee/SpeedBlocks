#ifndef MAINMENU_H
#define MAINMENU_H

#include "GuiBase.h"

class Resources;

class Menu : public GuiBase {
public:
	Menu(sf::Rect<int> _pos, Resources& _res);
	void Training();
	void quitGame();
};

#endif
