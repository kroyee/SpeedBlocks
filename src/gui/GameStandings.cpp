#include "GameStandings.h"
#include <SFML/Network.hpp>
#include "GameSignals.h"
#include "NetworkPackets.hpp"
#include "Resources.h"

using std::to_string;

static auto& AreThereFields = Signal<int, int>::get("AreThereFields");
static auto& SetFieldsSize = Signal<void, int, int>::get("SetFieldsSize");

GameStandings::GameStandings(sf::Rect<int> _pos, Resources& _res) : GuiBase(_pos, _res) {
    os::Label().pos(31, 0).text("Sets").text_size(18).add_to(panel);
    rounds.pos(16, 50).text("Rounds").text_size(18).add_to(panel);
    rounds.pos(10, 25).text("0").text_size(18).add_to(panel);
    rounds.pos(80, 25).text("0").text_size(18).add_to(panel);
    rounds.pos(10, 75).text("0").text_size(18).add_to(panel);
    rounds.pos(80, 75).text("0").text_size(18).add_to(panel);

    PM::handle_packet([&](const NP_GameScore& p) {
        const auto &p1_scores = p.scores[0], &p2_scores = p.scores[1];
        p1_id = p1_scores.id;
        _p1_sets = p1_scores.sets;
        _p1_rounds = p1_scores.rounds;

        p2_id = p2_scores.id;
        _p2_sets = p2_scores.sets;
        _p2_rounds = p2_scores.rounds;

        setResults();
    });
    PM::handle_packet([&](const NP_TournamentTimeWo& p) { setWaitTime(p.time); });
}

void GameStandings::setResults() {
    if (resources.gamestate == GameStates::Spectating)
        panel.pos(365, 195);
    else
        panel.pos(330, 185);

    if (resources.gamestate == GameStates::Spectating) {
        bool p2 = true;
        if (AreThereFields(1))
            if (p1_id == AreThereFields(1)) {
                p1_sets.text(to_string(_p1_sets));
                p2_sets.text(to_string(_p2_sets));
                p1_rounds.text(to_string(_p1_rounds));
                p2_rounds.text(to_string(_p2_rounds));
                p2 = false;
            }

        if (p2) {
            p2_sets.text(to_string(_p1_sets));
            p1_sets.text(to_string(_p2_sets));
            p2_rounds.text(to_string(_p1_rounds));
            p1_rounds.text(to_string(_p2_rounds));
        }
    } else {
        if (p1_id == resources.myId) {
            p1_sets.text(to_string(_p1_sets));
            p2_sets.text(to_string(_p2_sets));
            p1_rounds.text(to_string(_p1_rounds));
            p2_rounds.text(to_string(_p2_rounds));
        } else {
            p2_sets.text(to_string(_p1_sets));
            p1_sets.text(to_string(_p2_sets));
            p2_rounds.text(to_string(_p1_rounds));
            p1_rounds.text(to_string(_p2_rounds));
        }
    }

    if (_p1_rounds == 255) {
        p1_rounds.text("Game Over");
        p2_rounds.text("");
    }
    if (_p1_sets == 255) {
        p1_sets.text("Game Over");
        p2_sets.text("");
    }
    p1_rounds.pos(10, 75);
    show();
}

void GameStandings::setWaitTime(uint16_t time) {
    if (resources.gamestate == GameStates::Spectating)
        panel.pos(365, 195);
    else
        panel.pos(330, 185);

    p1_rounds.pos(0, 75);
    p1_rounds.text("WO in " + to_string(time) + "min");
    p2_rounds.text("");
    show();
}

void GameStandings::alignResult() {
    bool p2 = true;
    if (AreThereFields(1))
        if (p1_id == AreThereFields(1)) {
            p1_sets.text(to_string(_p1_sets));
            p2_sets.text(to_string(_p2_sets));
            p1_rounds.text(to_string(_p1_rounds));
            p2_rounds.text(to_string(_p2_rounds));
            p2 = false;
        }

    if (p2) {
        p2_sets.text(to_string(_p1_sets));
        p1_sets.text(to_string(_p2_sets));
        p2_rounds.text(to_string(_p1_rounds));
        p1_rounds.text(to_string(_p2_rounds));
    }

    if (_p1_rounds == 255) {
        p1_rounds.text("Game Over");
        p2_rounds.text("");
    }
    p1_rounds.pos(10, 75);

    if (AreThereFields(0) == 1)
        SetFieldsSize(450, 555);
    else
        SetFieldsSize(910, 555);
}
