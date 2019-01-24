#include "ScoreScreen.h"
#include "GameSignals.h"
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

    Net::takePacket(8, &ScoreScreen::getScores, this);

    connectSignal("AddLocalScore", &ScoreScreen::addRowLocal, this);
    connectSignal("SetRoundlenghtForScore", [&](int lenght) {
        roundLenght = lenght;
        clear();
    });
}

void ScoreScreen::clear() {}

void ScoreScreen::getScores(sf::Packet& packet) {
    uint8_t count;
    packet >> roundLenght >> count;
    list.pre_update();
    for (int i = 0; i < count; i++) addRow(packet);
    list.sort([](auto& lhs, auto& rhs) {
        auto l = static_cast<tgui::Label*>(lhs[1].get());
        auto r = static_cast<tgui::Label*>(rhs[1].get());
        return std::stoi(std::string(l->getText())) < std::stoi(std::string(r->getText()));
    });
    list.post_update();
}

void ScoreScreen::addRow(sf::Packet& packet) {
    ScoreRow score;

    packet >> score.id >> score.combo >> score.sent >> score.received >> score.blocked >> score.bpm;
    packet >> score.rank >> score.position >> score.score >> score.adj >> score.points;

    score.name = getName(score.id);
    score.spm = score.sent / (roundLenght / 60.0);
    score.apm = (score.sent + score.blocked) / (roundLenght / 60.0);

    setRowLabels(score, 1);
}

void ScoreScreen::addRowLocal(GameplayData& data, uint16_t id, const std::string& name, uint16_t _score) {
    ScoreRow score;

    score.name = name;
    score.id = id;
    score.combo = data.maxCombo;
    score.sent = data.linesSent;
    score.received = data.linesRecieved;
    score.blocked = data.linesBlocked;
    score.bpm = data.bpm;
    score.score = _score;

    score.spm = score.sent / (roundLenght / 60.0);
    score.apm = (score.sent + score.blocked) / (roundLenght / 60.0);

    setRowLabels(score, 2);
}

void ScoreScreen::setRowLabels(ScoreRow& score, uint8_t type) {
    score.exp.hide();
    score.labels[6].hide();

    if (type == 1) {
        score.exp.size(45, 15).min(0).max(2000).set(score.points).title(to_string(score.rank)).title_size(14).title_on_top();

        std::string rounding = to_string((int)score.adj);  // A bit messy-looking way of rounding float to 1 decimal
        rounding += "." + to_string((int)((score.adj - (int)score.adj) * 10));

        score.labels[6].text(rounding).text_size(14);

        score.exp.show();
        score.labels[6].show();
    }

    score.labels[0].text(score.name).text_size(14);
    score.labels[1].text(to_string(score.score)).text_size(14);
    score.labels[3].text(to_string(score.bpm)).text_size(14);
    score.labels[4].text(to_string(score.combo)).text_size(14);
    score.labels[5].text(to_string(score.sent)).text_size(14);
    score.labels[7].text(to_string(score.spm)).text_size(14);
    score.labels[8].text(to_string(score.apm)).text_size(14);
    score.labels[9].text(to_string(score.blocked) + "/" + to_string(score.received)).text_size(14);

    list.add(score.id, score.labels[0], score.labels[1], score.exp, score.labels[3], score.labels[4], score.labels[5], score.labels[6], score.labels[7], score.labels[8],
             score.labels[9]);
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
