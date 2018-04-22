#ifndef SCORESCREEN_H
#define SCORESCREEN_H

#include <TGUI/TGUI.hpp>
#include <SFML/Network.hpp>
#include "GuiBase.h"

class Resources;
struct GameplayData;

struct ScoreRow {
	std::string name;
	uint8_t combo, rank, position;
	uint16_t id, sent, received, blocked, bpm, spm, score, apm, points;
	float adj;

	tgui::Label::Ptr labels[10];
	tgui::ProgressBar::Ptr exp;
};

class ScoreScreen : public GuiBase {
private:
	tgui::Panel::Ptr scrollPanel;
	tgui::Panel::Ptr highlight;
	tgui::Scrollbar::Ptr scroll;
	uint16_t selected, roundLenght;
	uint8_t rowCount;
	std::vector<ScoreRow> scores;
public:
	std::string unknown;
	ScoreScreen(sf::Rect<int> _pos, Resources& _res);
	void clear();
	void getScores(sf::Packet& packet);
	void addRow(sf::Packet& packet);
	void addRowLocal(GameplayData & data, uint16_t id, const std::string& name, uint16_t score);
	void setRowLabels(ScoreRow& row, uint8_t type);
	void selectRow(uint8_t index);
	void scorePressed(sf::Vector2f pos);
	void scrolled(int scrollpos);
	void setItemPos();
	void handleEvent(sf::Event& event);
	const std::string& getName(uint16_t id);
};

#endif