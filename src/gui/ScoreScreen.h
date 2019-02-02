#ifndef SCORESCREEN_H
#define SCORESCREEN_H

#include <SFML/Network.hpp>
#include <TGUI/TGUI.hpp>
#include "GuiBase.h"
#include "ScrollList.hpp"

class Resources;
struct GameplayData;
struct RoundScoreServer;

struct ScoreRow {
    std::string name;
    uint8_t combo, rank, position;
    uint16_t id, sent, received, blocked, bpm, spm, score, apm, points;
    float adj;

    os::Label labels[10];
    os::ProgressBar exp;
};

class ScoreScreen : public GuiBase {
   private:
    os::ScrollList list;
    uint16_t selected, roundLenght;
    uint8_t rowCount;

   public:
    std::string unknown;
    ScoreScreen(sf::Rect<int> _pos, Resources& _res);
    void clear();
    void addRowLocal(GameplayData& data, uint16_t id, const std::string& name, uint16_t score);
    void setRowLabels(const RoundScoreServer& row, std::string name = "");
    void selectRow(uint8_t index);
    void scorePressed(sf::Vector2f pos);
    void scrolled(int scrollpos);
    void setItemPos();
    void handleEvent(sf::Event& event);
    const std::string& getName(uint16_t id);
};

#endif