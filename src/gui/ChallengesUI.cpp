#include "ChallengesUI.h"
#include "Signal.h"
#include <SFML/Network.hpp>
using std::cout;
using std::endl;
using std::to_string;

ChallengesUI::ChallengesUI(sf::Rect<int> _pos, Resources& _res, tgui::Panel::Ptr parentPanel) :
guiBase(_pos, _res, parentPanel),
challengeList(sf::Rect<int>(0,0,450,500), _res, panel) {
	
	challengeList.show();

	leaderPanel = tgui::Panel::create();
	leaderPanel->setPosition(420,0);
	leaderPanel->setSize(500,500);
	leaderPanel->setBackgroundColor(sf::Color(255,255,255,0));
	leaderPanel->hide();
	panel->add(leaderPanel);

	title = resources.gfx->themeTG->load("Label");
	title->setPosition(113,0);
	title->setText("Leader board");
	title->setTextSize(33);
	leaderPanel->add(title);

	scrollPanel = tgui::Panel::create();
	scrollPanel->setPosition(0,60);
	scrollPanel->setSize(470,360);
	scrollPanel->setBackgroundColor(sf::Color(255,255,255,0));
	leaderPanel->add(scrollPanel);

	scroll = resources.gfx->themeTG->load("Scrollbar");
	scroll->setSize(30, 360);
	scroll->setPosition(470, 60);
	scroll->setMaximum(2);
	scroll->setLowValue(1);
	scroll->connect("ValueChanged", &ChallengesUI::listScrolled, this);
	leaderPanel->add(scroll);

	playChallenge = resources.gfx->themeTG->load("Button");
	playChallenge->setSize(180, 50);
	playChallenge->setPosition(160, 430);
	playChallenge->setText("Play challenge");
	playChallenge->connect("pressed", &ChallengesUI::play, this);

	Net::takePacket(2, &ChallengesUI::makeList, this);
	Net::takePacket(5, &ChallengesUI::makeLeaderboard, this);
}

void ChallengesUI::makeList(sf::Packet &packet) {
	sf::Uint8 count;
	packet >> count;

	challengeList.removeAllItems();

	sf::Uint16 id;
	sf::String name, label;
	for (int i=0; i<count; i++) {
		packet >> id >> name >> label;
		challengeList.addItem(name, label, id);
	}
}

void ChallengesUI::makeLeaderboard(sf::Packet &packet) {
	sf::String oldTitle = title->getText();
	int scrollpos = scroll->getValue();

	leaderPanel->removeAllWidgets();
	leaderPanel->add(title);
	leaderPanel->add(scroll);
	leaderPanel->add(scrollPanel);
	leaderPanel->add(playChallenge);
	leaderPanel->show();
	packet >> selectedId >> columns;
	for (auto&& chall : challengeList.items)
		if (chall.id == selectedId)
			title->setText(chall.name);

	width[0] = 0;
	columns++;
	sf::String string;
	for (int i=1; i<columns; i++) {
		packet >> width[i];
		width[i]+=50;

		packet >> string;
		tgui::Label::Ptr label = resources.gfx->themeTG->load("Label");
		label->setPosition(width[i], 40);
		label->setText(string);
		label->setTextSize(16);
		leaderPanel->add(label);
	}

	tgui::Label::Ptr position = resources.gfx->themeTG->load("Label");
	position->setPosition(0, 40);
	position->setText("#");
	position->setTextSize(16);
	leaderPanel->add(position);

	scrollPanel->removeAllWidgets();
	rows.clear();
	ChallengesRow row;
	packet >> itemsInScrollPanel;
	for (sf::Uint16 c=0; c<itemsInScrollPanel; c++) {

		row.label[0] = resources.gfx->themeTG->load("Label");
		row.label[0]->setPosition(0, c*30+7);
		row.label[0]->setText(to_string(c+1));
		row.label[0]->setTextSize(14);
		scrollPanel->add(row.label[0]);

		for (sf::Uint8 i=1; i<columns; i++) {
			packet >> string;
			row.label[i] = resources.gfx->themeTG->load("Label");
			row.label[i]->setPosition(width[i], c*30+7);
			row.label[i]->setText(string);
			row.label[i]->setTextSize(14);
			scrollPanel->add(row.label[i]);
		}
		row.button = resources.gfx->themeTG->load("Button");
		row.button->setText("View");
		row.button->setPosition(400, c*30+5);
		row.button->setSize(45, 20);
		row.button->connect("pressed", &ChallengesUI::viewReplay, this, c);
		rows.push_back(row);
		scrollPanel->add(row.button);
		scrollPanel->show();
	}

	int height = itemsInScrollPanel*30+7;
	if (height <= scrollPanel->getSize().y)
		scroll->setMaximum(0);
	else {
		height-=scrollPanel->getSize().y;
		height/=30;
		height++;
		scroll->setMaximum(height);
	}

	scroll->setValue(0);
	if (oldTitle == title->getText())
		scroll->setValue(scrollpos);
}

void ChallengesUI::play() {
	Signals::SendSig(17, selectedId);
}

void ChallengesUI::show() {
	panel->show();
	leaderPanel->hide();
}

void ChallengesUI::viewReplay(sf::Uint16 slot) {
	Signals::SendSig(18, selectedId, slot);
}

void ChallengesUI::listScrolled(int scrollpos) {
	for (auto it = rows.begin(); it != rows.end(); it++) {
		int i = std::distance(rows.begin(), it);
		it->button->setPosition(400, i*30+5 - scrollpos*30);
		for (sf::Uint8 c=0; c<columns; c++)
			it->label[c]->setPosition(width[c], i*30+7 - scrollpos*30);
	}
}

void ChallengesUI::scrolled(sf::Event& event) {
	if (leaderPanel->isVisible())
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