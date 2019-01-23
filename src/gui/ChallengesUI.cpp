#include "ChallengesUI.h"
#include <SFML/Network.hpp>
#include <vector>
#include "GameSignals.h"
#include "Resources.h"

static auto& QuickMsg = Signal<void, const std::string&>::get("QuickMsg");
static auto& SendRecording = Signal<void, int>::get("SendRecording");

ChallengesUI::ChallengesUI(sf::Rect<int> _pos, Resources& _res, tgui::Panel::Ptr parentPanel)
    : GuiBase(_pos, _res, parentPanel), challengeList(sf::Rect<int>(0, 0, 450, 500), _res, panel.get()) {
    challengeList.show();

    leaderPanel.pos(420, 0).size(500, 500).hide().add_to(panel);

    title.pos(113, 0).text("Leader board").text_size(32).add_to(leaderPanel);

    scrollPanel.pos(0, 60).size(470, 390).add_to(leaderPanel);

    playChallenge.size(180, 50).pos(160, 430).text("Play challenge").connect("pressed", &ChallengesUI::play, this).add_to(leaderPanel);

    Net::takePacket(2, &ChallengesUI::makeList, this);
    Net::takePacket(5, &ChallengesUI::makeLeaderboard, this);
    Net::takePacket(6, [&](sf::Packet& packet) {
        std::string text;
        packet >> text;
        QuickMsg("You improved your score from " + text);
        SendRecording(selectedId);
    });
}

void ChallengesUI::makeList(sf::Packet& packet) {
    uint8_t count;
    packet >> count;

    challengeList.removeAllItems();

    uint16_t id;
    std::string name, label;
    for (int i = 0; i < count; i++) {
        packet >> id >> name >> label;
        challengeList.addItem(name, label, id);
    }
}

void ChallengesUI::makeLeaderboard(sf::Packet& packet) {
    leaderPanel.show();
    uint8_t columns;
    packet >> selectedId >> columns;
    for (auto&& chall : challengeList.items)
        if (chall.id == selectedId) title->setText(chall.name);

    std::vector<int> label_positions;
    label_positions.push_back(0);
    columns++;
    std::string string;

    for (int i = 1; i < columns; i++) {
        uint16_t width;
        packet >> width;
        width += 50;
        label_positions.push_back(width);

        packet >> string;
        os::Label().pos(width, 40).text(string).text_size(16).add_to(leaderPanel);
    }

    label_positions.push_back(400);
    scrollPanel.set_positions(label_positions);

    os::Label().text("#").pos(0, 40).text_size(16).add_to(leaderPanel);

    uint16_t itemsInScrollPanel;
    packet >> itemsInScrollPanel;
    for (uint16_t c = 0; c < itemsInScrollPanel; c++) {
        std::vector<tgui::Widget::Ptr> row;

        row.push_back(os::Label().text(std::to_string(c + 1)).text_size(14).get());

        for (uint8_t i = 1; i < columns; i++) {
            packet >> string;
            row.push_back(os::Label().text(string).text_size(14).get());
        }

        row.push_back(os::Button().text("View").size(45, 20).connect("pressed", &ChallengesUI::viewReplay, this, c).get());

        scrollPanel.add(c, std::move(row));
    }
    while (scrollPanel.size() > itemsInScrollPanel) {
        scrollPanel.pop_back();
    }
    scrollPanel.show();
}

void ChallengesUI::play() { SendSignal(17, selectedId); }

void ChallengesUI::show() {
    panel.show();
    leaderPanel.hide();
}

void ChallengesUI::viewReplay(uint16_t slot) { SendSignal(18, selectedId, slot); }