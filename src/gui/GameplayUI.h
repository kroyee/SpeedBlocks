#ifndef GAMEPLAYUI_H
#define GAMEPLAYUI_H

#include "guiBase.h"

class UI;

class PrivChat {
public:
	tgui::ChatBox::Ptr chatBox;
	sf::String name;
};

class GameplayUI : public guiBase {
public:
	tgui::Tab::Ptr InGameTab;
	tgui::Tab::Ptr ChatTab;

	tgui::Panel::Ptr GameFields;
	tgui::Panel::Ptr Score;
	tgui::Panel::Ptr Chat;

	tgui::ChatBox::Ptr Room;
	tgui::ChatBox::Ptr Lobby;
	tgui::EditBox::Ptr ChatBox;

	std::vector<PrivChat> privChats;

	short scoreRows;

	sf::String unknown;

	void create(sf::Rect<int> _pos, UI* _gui);
	void igtabSelect(const std::string& tab);
	void chattabSelect(const std::string& tab);
	void scoreRow();
	void clearScore();

	const sf::String& getName(sf::Uint16 id);
};

#endif