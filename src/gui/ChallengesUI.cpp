#include "ChallengesUI.h"
#include <SFML/Network.hpp>
#include <vector>
#include "GameSignals.h"
#include "NetworkPackets.hpp"
#include "Resources.h"

static auto& QuickMsg = Signal<void, const std::string&>::get("QuickMsg");
static auto& SendRecording = Signal<void, int>::get("SendRecording");

ChallengesUI::ChallengesUI(sf::Rect<int> _pos, Resources& _res, os::Panel& parentPanel)
    : GuiBase(_pos, _res, parentPanel), challengeList(sf::Rect<int>(0, 0, 450, 500), _res, panel) {
    challengeList.show();

    leaderPanel.pos(420, 0).size(500, 500).hide().add_to(panel);

    title.pos(113, 0).text("Leader board").text_size(32).add_to(leaderPanel);

    scrollPanel.pos(0, 60).size(470, 390).add_to(leaderPanel);

    playChallenge.size(180, 50).pos(160, 430).text("Play challenge").connect("pressed", &ChallengesUI::play, this).add_to(leaderPanel);

    PM::handle_packet([&](const NP_ChallengeList& p) {
        challengeList.removeAllItems();
        for (auto& c : p.challenges) challengeList.addItem(c.name, c.label, c.id);
    });
    PM::handle_packet([&](const NP_ChallengeLeaderboard& p) {
        leaderPanel.show();

        std::vector<int> label_positions;
        for (auto& c : p.columns) {
            label_positions.push_back(c.x_pos);
            os::Label().pos(c.x_pos, 40).text(c.text).text_size(16).add_to(leaderPanel);
        }
        scrollPanel.set_positions(label_positions);

        unsigned i;
        for (i = 0; i < p.scores.size(); ++i) {
            std::vector<tgui::Widget::Ptr> row;
            row.push_back(os::Label().text(std::to_string(i + 1)).text_size(14).get());
            row.push_back(os::Label().text(p.scores[i].name).text_size(14).get());
            for (auto& s : p.scores[i].scores) {
                row.push_back(os::Label().text(s.text).text_size(14).get());
            }

            row.push_back(os::Button().text("View").size(45, 20).connect("pressed", &ChallengesUI::viewReplay, this, i).get());

            scrollPanel.add(i, std::move(row));
        }

        while (scrollPanel.size() > i) {
            scrollPanel.pop_back();
        }

        scrollPanel.show();
    });
    PM::handle_packet([&](const NP_ReplayRequest& p) {
        QuickMsg("You improved your score from " + p.message);
        SendRecording(selectedId);
    });
}

void ChallengesUI::play() { SendSignal(17, selectedId); }

void ChallengesUI::show() {
    panel.show();
    leaderPanel.hide();
}

void ChallengesUI::viewReplay(uint16_t slot) { SendSignal(18, selectedId, slot); }