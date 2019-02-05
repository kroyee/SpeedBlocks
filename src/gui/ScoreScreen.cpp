#include "ScoreScreen.h"
#include "GameSignals.h"
#include "Packets.hpp"
#include "Resources.h"

using std::to_string;

ScoreScreen::ScoreScreen(sf::Rect<int> _pos, Resources& _res) : GuiBase(_pos, _res) {
    panel.background(sf::Color(100, 100, 100, 200));

    unknown = "Unknown";

    list.pos(0, 30).size(840, 510).add_to(panel);
    list.set_positions(5, 65, 110, 155, 200, 240, 275, 315, 355, 420);

    os::Label().text("Name").pos(5, 5).text_size(14).add_to(panel);
    os::Label().text("Score").pos(65, 5).size(100, 25).text_size(14).add_to(panel);
    os::Label().text("Rank").pos(110, 5).size(100, 25).text_size(14).add_to(panel);
    os::Label().text("BPM").pos(155, 5).size(100, 25).text_size(14).add_to(panel);
    os::Label().text("Cmb").pos(200, 5).size(100, 25).text_size(14).add_to(panel);
    os::Label().text("Sent").pos(240, 5).size(100, 25).text_size(14).add_to(panel);
    os::Label().text("Adj").pos(275, 5).size(100, 25).text_size(14).add_to(panel);
    os::Label().text("SPM").pos(315, 5).size(100, 25).text_size(14).add_to(panel);
    os::Label().text("APM").pos(355, 5).size(100, 25).text_size(14).add_to(panel);
    os::Label().text("Blocked").pos(420, 5).size(100, 25).text_size(14).add_to(panel);

    rowCount = 0;

    PM::handle_packet([&](const NP_RoomScore& p) {
        list.pre_update();
        for (auto& score : p.scores) setRowLabels(score, "");
        list.sort([](auto& lhs, auto& rhs) {
            auto l = static_cast<tgui::Label*>(lhs[1].get());
            auto r = static_cast<tgui::Label*>(rhs[1].get());
            return std::stoi(std::string(l->getText())) < std::stoi(std::string(r->getText()));
        });
        list.post_update();
    });

    connectSignal("AddLocalScore", &ScoreScreen::addRowLocal, this);
    connectSignal("SetRoundlenghtForScore", [&](int lenght) {
        roundLenght = lenght;
        clear();
    });
}

void ScoreScreen::clear() {}

void ScoreScreen::addRowLocal(GameplayData& data, uint16_t id, const std::string& name, uint16_t _score) {
    RoundScoreServer score;

    score.id = id;
    score.score.max_combo = data.maxCombo;
    score.score.lines_sent = data.linesSent;
    score.score.lines_recieved = data.linesRecieved;
    score.score.lines_blocked = data.linesBlocked;
    score.score.bpm = data.bpm;
    score.game_score = _score;

    setRowLabels(score, name);
}

void ScoreScreen::setRowLabels(const RoundScoreServer& score, std::string name) {
    auto exp = os::ProgressBar().size(45, 15).min(0).max(2000).set(score.ffa_points).title(to_string(score.ffa_rank)).title_size(14).title_on_top();

    std::string rounding = to_string((int)score.lines_adjusted);  // A bit messy-looking way of rounding float to 1 decimal
    rounding += "." + to_string((int)((score.lines_adjusted - (int)score.lines_adjusted) * 10));
    auto l6 = os::Label().text(rounding).text_size(14);

    if (name == "") {
        name = getName(score.id);
    } else {
        exp.hide();
        l6.hide();
    }

    auto spm = score.score.lines_sent / (roundLenght / 60.0);
    auto apm = (score.score.lines_sent + score.score.lines_blocked) / (roundLenght / 60.0);

    auto l0 = os::Label().text(name).text_size(14);
    auto l1 = os::Label().text(to_string(score.game_score)).text_size(14);
    auto l3 = os::Label().text(to_string(score.score.bpm)).text_size(14);
    auto l4 = os::Label().text(to_string(score.score.max_combo)).text_size(14);
    auto l5 = os::Label().text(to_string(score.score.lines_sent)).text_size(14);
    auto l7 = os::Label().text(to_string(spm)).text_size(14);
    auto l8 = os::Label().text(to_string(apm)).text_size(14);
    auto l9 = os::Label().text(to_string(score.score.lines_blocked) + "/" + to_string(score.score.lines_recieved)).text_size(14);

    list.add(score.id, l0, l1, exp, l3, l4, l5, l6, l7, l8, l9);
}

void ScoreScreen::handleEvent(sf::Event& event) {
    if (event.type == sf::Event::KeyPressed)
        if (event.key.code == sf::Keyboard::Tab) panel.show();
    if (event.type == sf::Event::KeyReleased)
        if (event.key.code == sf::Keyboard::Tab) panel.hide();
}

const std::string& ScoreScreen::getName(uint16_t id) {
    for (auto& client : resources.clientList)
        if (client.id == id) return client.name;
    return unknown;
}
