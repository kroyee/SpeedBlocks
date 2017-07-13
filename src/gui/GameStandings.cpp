#include "GameStandings.h"
#include "gui.h"
#include "gameField.h"
using std::to_string;

void GameStandings::create(sf::Rect<int> _pos, UI* _ui) {
	createBase(_pos, _ui);

	tgui::Label::Ptr widget0 = gui->themeTG->load("Label");
	widget0->setPosition(31,0);
	widget0->setText("Sets");
	widget0->setTextSize(18);
	panel->add(widget0);

	rounds = gui->themeTG->load("Label");
	rounds->setPosition(16,50);
	rounds->setText("Rounds");
	rounds->setTextSize(18);
	panel->add(rounds);

	p1_sets = gui->themeTG->load("Label");
	p1_sets->setPosition(10,25);
	p1_sets->setText("0");
	p1_sets->setTextSize(18);
	panel->add(p1_sets);

	p2_sets = gui->themeTG->load("Label");
	p2_sets->setPosition(80,25);
	p2_sets->setText("0");
	p2_sets->setTextSize(18);
	panel->add(p2_sets);

	p1_rounds = gui->themeTG->load("Label");
	p1_rounds->setPosition(10,75);
	p1_rounds->setText("0");
	p1_rounds->setTextSize(18);
	panel->add(p1_rounds);

	p2_rounds = gui->themeTG->load("Label");
	p2_rounds->setPosition(80,75);
	p2_rounds->setText("0");
	p2_rounds->setTextSize(18);
	panel->add(p2_rounds);
}

void GameStandings::setResults() {
	if (gui->gamestate == Spectating)
		panel->setPosition(365, 195);
	else
		panel->setPosition(330, 185);

    gui->net.packet >> p1_id >> p2_id >> _p1_sets >> _p2_sets >> _p1_rounds >> _p2_rounds;

    if (gui->gamestate == Spectating) {
    	bool p2=true;
    	if (gui->gameFieldDrawer.fields.size())
	    	if (p1_id == gui->gameFieldDrawer.fields.front().id) {
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
	    if (p1_id == gui->myId) {
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

void GameStandings::setWaitTime(sf::Uint16 time) {
	if (gui->gamestate == Spectating)
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
	if (gui->gameFieldDrawer.fields.size())
    	if (p1_id == gui->gameFieldDrawer.fields.front().id) {
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

    if (gui->gameFieldDrawer.fields.size() == 1)
    	gui->gameFieldDrawer.setSize(470, 555);
    else
    	gui->gameFieldDrawer.setSize(950, 555);
}