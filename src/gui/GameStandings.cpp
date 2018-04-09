#include "GameStandings.h"
#include "GameSignals.h"
#include <SFML/Network.hpp>
using std::to_string;

static auto& AreThereFields = Signal<int, int>::get("AreThereFields");
static auto& SetFieldsSize = Signal<void, int, int>::get("SetFieldsSize");

GameStandings::GameStandings(sf::Rect<int> _pos, Resources& _res) : guiBase(_pos, _res) {

	tgui::Label::Ptr widget0 = resources.gfx->themeTG->load("Label");
	widget0->setPosition(31,0);
	widget0->setText("Sets");
	widget0->setTextSize(18);
	panel->add(widget0);

	rounds = resources.gfx->themeTG->load("Label");
	rounds->setPosition(16,50);
	rounds->setText("Rounds");
	rounds->setTextSize(18);
	panel->add(rounds);

	p1_sets = resources.gfx->themeTG->load("Label");
	p1_sets->setPosition(10,25);
	p1_sets->setText("0");
	p1_sets->setTextSize(18);
	panel->add(p1_sets);

	p2_sets = resources.gfx->themeTG->load("Label");
	p2_sets->setPosition(80,25);
	p2_sets->setText("0");
	p2_sets->setTextSize(18);
	panel->add(p2_sets);

	p1_rounds = resources.gfx->themeTG->load("Label");
	p1_rounds->setPosition(10,75);
	p1_rounds->setText("0");
	p1_rounds->setTextSize(18);
	panel->add(p1_rounds);

	p2_rounds = resources.gfx->themeTG->load("Label");
	p2_rounds->setPosition(80,75);
	p2_rounds->setText("0");
	p2_rounds->setTextSize(18);
	panel->add(p2_rounds);

	Net::takePacket(24, &GameStandings::setResults, this);
	Net::takeSignal(3, &GameStandings::setWaitTime, this);
}

void GameStandings::setResults(sf::Packet &packet) {
	if (resources.gamestate == GameStates::Spectating)
		panel->setPosition(365, 195);
	else
		panel->setPosition(330, 185);

    packet >> p1_id >> p2_id >> _p1_sets >> _p2_sets >> _p1_rounds >> _p2_rounds;

    if (resources.gamestate == GameStates::Spectating) {
    	bool p2=true;
    	if (AreThereFields(1))
	    	if (p1_id == AreThereFields(1)) {
		        p1_sets->setText(to_string(_p1_sets));
		        p2_sets->setText(to_string(_p2_sets));
		        p1_rounds->setText(to_string(_p1_rounds));
		        p2_rounds->setText(to_string(_p2_rounds));
		        p2=false;
		    }

	    if (p2) {
	        p2_sets->setText(to_string(_p1_sets));
	        p1_sets->setText(to_string(_p2_sets));
	        p2_rounds->setText(to_string(_p1_rounds));
	        p1_rounds->setText(to_string(_p2_rounds));
	    }
    }
    else {
	    if (p1_id == resources.myId) {
	        p1_sets->setText(to_string(_p1_sets));
	        p2_sets->setText(to_string(_p2_sets));
	        p1_rounds->setText(to_string(_p1_rounds));
	        p2_rounds->setText(to_string(_p2_rounds));
	    }
	    else {
	        p2_sets->setText(to_string(_p1_sets));
	        p1_sets->setText(to_string(_p2_sets));
	        p2_rounds->setText(to_string(_p1_rounds));
	        p1_rounds->setText(to_string(_p2_rounds));
	    }
	}

    if (_p1_rounds == 255) {
        p1_rounds->setText("Game Over");
        p2_rounds->setText("");
    }
    if (_p1_sets == 255) {
    	p1_sets->setText("Game Over");
    	p2_sets->setText("");
    }
    p1_rounds->setPosition(10,75);
    show();
}

void GameStandings::setWaitTime(uint16_t time) {
	if (resources.gamestate == GameStates::Spectating)
		panel->setPosition(365, 195);
	else
		panel->setPosition(330, 185);

    p1_rounds->setPosition(0,75);
    p1_rounds->setText("WO in " + to_string(time) + "min");
    p2_rounds->setText("");
    show();
}

void GameStandings::alignResult() {
	bool p2=true;
	if (AreThereFields(1))
    	if (p1_id == AreThereFields(1)) {
	        p1_sets->setText(to_string(_p1_sets));
	        p2_sets->setText(to_string(_p2_sets));
	        p1_rounds->setText(to_string(_p1_rounds));
	        p2_rounds->setText(to_string(_p2_rounds));
	        p2=false;
	    }

    if (p2) {
        p2_sets->setText(to_string(_p1_sets));
        p1_sets->setText(to_string(_p2_sets));
        p2_rounds->setText(to_string(_p1_rounds));
        p1_rounds->setText(to_string(_p2_rounds));
    }

    if (_p1_rounds == 255) {
        p1_rounds->setText("Game Over");
        p2_rounds->setText("");
    }
    p1_rounds->setPosition(10,75);

    if (AreThereFields(0) == 1)
    	SetFieldsSize(450, 555);
    else
    	SetFieldsSize(910, 555);
}