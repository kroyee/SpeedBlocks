#ifndef SCORESCREEN_H
#define SCORESCREEN_H

#include <TGUI/TGUI.hpp>
#include <SFML/Network.hpp>
#include "guiBase.h"

class Resources;
struct GameplayData;

struct ScoreRow {
	sf::String name;
	sf::Uint8 combo, rank, position;
	sf::Uint16 id, sent, received, blocked, bpm, spm, score, apm, points;
	float adj;

	tgui::Label::Ptr labels[10];
	tgui::ProgressBar::Ptr exp;
};

class ScoreScreen : public guiBase {
private:
	tgui::Panel::Ptr scrollPanel;
	tgui::Panel::Ptr highlight;
	tgui::Scrollbar::Ptr scroll;
	sf::Uint16 selected, roundLenght;
	sf::Uint8 rowCount;
	std::vector<ScoreRow> scores;
public:
	sf::String unknown;
	ScoreScreen(sf::Rect<int> _pos, Resources& _res);
	void clear();
	void getScores(sf::Packet& packet);
	void addRow(sf::Packet& packet);
	void addRowLocal(GameplayData & data, uint16_t id, const sf::String& name, uint16_t score);
	void setRowLabels(ScoreRow& row, uint8_t type);
	void selectRow(sf::Uint8 index);
	void scorePressed(sf::Vector2f pos);
	void scrolled(int scrollpos);
	void setItemPos();
	void handleEvent(sf::Event& event);
	const sf::String& getName(sf::Uint16 id);
};

#endif