#ifndef REPLAYUI_H
#define REPLAYUI_H

#include "guiBase.h"

class Resources;

class ReplayUI : public guiBase {
public:
	tgui::Label::Ptr timeTotal;
	tgui::Button::Ptr playPause;
	tgui::Button::Ptr gameForward;
	tgui::Button::Ptr setForward;
	tgui::Button::Ptr gameBack;
	tgui::Button::Ptr setBack;
	tgui::Label::Ptr timePlayed;
	tgui::ProgressBar::Ptr seekbar;
	tgui::Label::Ptr rounds;
	tgui::Label::Ptr sets;

	sf::Time pauseTime;

	std::string nameBackup;
	bool backup;

	ReplayUI(sf::Rect<int> _pos, Resources& _res);
	void show(bool showTournamentControls=false);
	void hide();

	void update(sf::Time);
	void seek(sf::Vector2f mouse);
	std::string displayTime(uint16_t timeVal);
	void pause();
};

#endif