#include "ScoreScreen.h"
#include "GameSignals.h"
#include "Resources.h"
using std::to_string;

ScoreScreen::ScoreScreen(sf::Rect<int> _pos, Resources& _res) : GuiBase(_pos, _res) {
	panel->setBackgroundColor(sf::Color(100, 100, 100, 200));

	unknown = "Unknown";

	highlight = tgui::Panel::create();
	highlight->setSize(840, 30);
	highlight->setBackgroundColor(sf::Color(150, 150, 150, 150));
	highlight->hide();
	panel->add(highlight);

	scrollPanel = tgui::Panel::create();
	scrollPanel->setPosition(0, 30);
	scrollPanel->setSize(840, 510);
	scrollPanel->setBackgroundColor(sf::Color(255, 255, 255, 0));
	scrollPanel->connect("MousePressed", &ScoreScreen::scorePressed, this);
	panel->add(scrollPanel);

	scroll = resources.gfx->load("Scrollbar");
	scroll->setSize(20, 540);
	scroll->setPosition(840, 0);
	scroll->setMaximum(0);
	scroll->setLowValue(17);
	scroll->connect("ValueChanged", &ScoreScreen::scrolled, this);
	panel->add(scroll);

	tgui::Label::Ptr label = resources.gfx->load("Label");
	label->setText("Name");
	label->setPosition(5, 5);
	label->setTextSize(14);
	panel->add(label);

	label = resources.gfx->load("Label");
	label->setText("Score");
	label->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Right);
	label->setPosition(65, 5);
	label->setSize(100, 25);
	label->setTextSize(14);
	panel->add(label);

	label = resources.gfx->load("Label");
	label->setText("Rank");
	label->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Right);
	label->setPosition(110, 5);
	label->setSize(100, 25);
	label->setTextSize(14);
	panel->add(label);

	label = resources.gfx->load("Label");
	label->setText("BPM");
	label->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Right);
	label->setPosition(155, 5);
	label->setSize(100, 25);
	label->setTextSize(14);
	panel->add(label);

	label = resources.gfx->load("Label");
	label->setText("Cmb");
	label->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Right);
	label->setPosition(200, 5);
	label->setSize(100, 25);
	label->setTextSize(14);
	panel->add(label);

	label = resources.gfx->load("Label");
	label->setText("Sent");
	label->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Right);
	label->setPosition(240, 5);
	label->setSize(100, 25);
	label->setTextSize(14);
	panel->add(label);

	label = resources.gfx->load("Label");
	label->setText("Adj");
	label->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Right);
	label->setPosition(275, 5);
	label->setSize(100, 25);
	label->setTextSize(14);
	panel->add(label);

	label = resources.gfx->load("Label");
	label->setText("SPM");
	label->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Right);
	label->setPosition(315, 5);
	label->setSize(100, 25);
	label->setTextSize(14);
	panel->add(label);

	label = resources.gfx->load("Label");
	label->setText("APM");
	label->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Right);
	label->setPosition(355, 5);
	label->setSize(100, 25);
	label->setTextSize(14);
	panel->add(label);

	label = resources.gfx->load("Label");
	label->setText("Blocked");
	label->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Right);
	label->setPosition(420, 5);
	label->setSize(100, 25);
	label->setTextSize(14);
	panel->add(label);

	rowCount=0;

	Net::takePacket(8, &ScoreScreen::getScores, this);

	connectSignal("AddLocalScore", &ScoreScreen::addRowLocal, this);
	connectSignal("SetRoundlenghtForScore", [&](int lenght){ roundLenght = lenght; clear(); });
}

void ScoreScreen::clear() {
	scores.clear();
	scrollPanel->removeAllWidgets();
	highlight->hide();

	rowCount=0;
}

void ScoreScreen::getScores(sf::Packet& packet) {
	uint8_t count;
	packet >> roundLenght >> count;
	clear();
	for (int i=0; i<count; i++)
		addRow(packet);
	setItemPos();
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

void ScoreScreen::addRowLocal(GameplayData & data, uint16_t id, const std::string& name, uint16_t _score) {
	ScoreRow score;

	score.name = name;
	score.id = id;						score.combo = data.maxCombo;
	score.sent = data.linesSent;		score.received = data.linesRecieved;
	score.blocked = data.linesBlocked;	score.bpm = data.bpm;
	score.score = _score;

	score.spm = score.sent / (roundLenght / 60.0);
	score.apm = (score.sent + score.blocked) / (roundLenght / 60.0);

	setRowLabels(score, 2);
}

void ScoreScreen::setRowLabels(ScoreRow& score, uint8_t type) {
	for (int i=0; i<10; i++)
		score.labels[i] = resources.gfx->load("Label");

	if (type == 1) {
		score.exp = resources.gfx->load("ProgressBar");
		score.exp->setPosition(172, rowCount*30+5);
		score.exp->setSize(45, 15);
		score.exp->setMinimum(0);
		score.exp->setMaximum(2000);
		score.exp->setValue(score.points);
		scrollPanel->add(score.exp);

		score.labels[2]->setText(to_string(score.rank));
		score.labels[2]->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Center);
		score.labels[2]->setPosition(172, rowCount*30+5);
		score.labels[2]->setSize(45, 30);
		score.labels[2]->setTextSize(14);
		scrollPanel->add(score.labels[2]);

		std::string rounding = to_string((int)score.adj); //A bit messy-looking way of rounding float to 1 decimal
		rounding += "." + to_string((int)((score.adj - (int)score.adj)*10));

		score.labels[6]->setText(rounding);
		score.labels[6]->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Right);
		score.labels[6]->setPosition(275, rowCount*30+5);
		score.labels[6]->setSize(100, 30);
		score.labels[6]->setTextSize(14);
		scrollPanel->add(score.labels[6]);
	}

	score.labels[0]->setText(score.name);
	score.labels[0]->setPosition(5, rowCount*30+5);
	score.labels[0]->setTextSize(14);
	scrollPanel->add(score.labels[0]);

	score.labels[1]->setText(to_string(score.score));
	score.labels[1]->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Right);
	score.labels[1]->setPosition(65, rowCount*30+5);
	score.labels[1]->setSize(100, 30);
	score.labels[1]->setTextSize(14);
	scrollPanel->add(score.labels[1]);

	score.labels[3]->setText(to_string(score.bpm));
	score.labels[3]->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Right);
	score.labels[3]->setPosition(155, rowCount*30+5);
	score.labels[3]->setSize(100, 30);
	score.labels[3]->setTextSize(14);
	scrollPanel->add(score.labels[3]);

	score.labels[4]->setText(to_string(score.combo));
	score.labels[4]->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Right);
	score.labels[4]->setPosition(200, rowCount*30+5);
	score.labels[4]->setSize(100, 30);
	score.labels[4]->setTextSize(14);
	scrollPanel->add(score.labels[4]);

	score.labels[5]->setText(to_string(score.sent));
	score.labels[5]->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Right);
	score.labels[5]->setPosition(240, rowCount*30+5);
	score.labels[5]->setSize(100, 30);
	score.labels[5]->setTextSize(14);
	scrollPanel->add(score.labels[5]);

	score.labels[7]->setText(to_string(score.spm));
	score.labels[7]->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Right);
	score.labels[7]->setPosition(315, rowCount*30+5);
	score.labels[7]->setSize(100, 30);
	score.labels[7]->setTextSize(14);
	scrollPanel->add(score.labels[7]);

	score.labels[8]->setText(to_string(score.apm));
	score.labels[8]->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Right);
	score.labels[8]->setPosition(355, rowCount*30+5);
	score.labels[8]->setSize(100, 30);
	score.labels[8]->setTextSize(14);
	scrollPanel->add(score.labels[8]);

	score.labels[9]->setText(to_string(score.blocked) + "/" + to_string(score.received));
	score.labels[9]->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Right);
	score.labels[9]->setPosition(420, rowCount*30+5);
	score.labels[9]->setSize(100, 30);
	score.labels[9]->setTextSize(14);
	scrollPanel->add(score.labels[9]);

	scores.push_back(score);

	if (score.id == selected)
		selectRow(rowCount);
	rowCount++;
}

void ScoreScreen::selectRow(uint8_t index) {
	if (index >= scores.size()) {
		selected=0;
		highlight->hide();
	}
	else {
		highlight->setPosition(2, index*30+30);
		highlight->show();
		selected = scores[index].id;
	}
}

void ScoreScreen::scorePressed(sf::Vector2f pos) {
	uint8_t index=0;
	while (pos.y > 30) {
		pos.y-=30;
		index++;
	}
	selectRow(index);
}

void ScoreScreen::setItemPos() {
	scroll->setMaximum(scores.size());
	scrolled(scroll->getValue());
}

void ScoreScreen::scrolled(int scrollpos) {
	int c=0;
	for (auto score : scores) {
		for (int i=0; i<10; i++)
			score.labels[i]->setPosition(score.labels[i]->getPosition().x, c*30+5 - scrollpos*30);
		score.exp->setPosition(score.exp->getPosition().x, c*30+5 - scrollpos*30);
		c++;
	}
}

void ScoreScreen::handleEvent(sf::Event& event) {
	if (event.type == sf::Event::KeyPressed)
		if (event.key.code == sf::Keyboard::Tab)
			panel->show();
	if (event.type == sf::Event::KeyReleased)
		if (event.key.code == sf::Keyboard::Tab)
			panel->hide();
	if (panel->isVisible())
		if (event.type == sf::Event::MouseWheelScrolled)
			if (event.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel)
				if (mouseOver(scrollPanel, event.mouseWheelScroll.x, event.mouseWheelScroll.y)) {
					short cur = scroll->getValue();
					cur-=event.mouseWheelScroll.delta;
					if (cur<0)
						cur=0;
					scroll->setValue(cur);
				}
}

const std::string& ScoreScreen::getName(uint16_t id) {
	for (auto& client : resources.clientList)
		if (client.id == id)
			return client.name;
	return unknown;
}
