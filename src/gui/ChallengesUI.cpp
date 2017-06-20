#include "ChallengesUI.h"
#include "gui.h"
using std::cout;
using std::endl;

void ChallengesUI::create(sf::Rect<int> _pos, UI* _ui, tgui::Panel::Ptr parentPanel) {
	createBase(_pos, _ui, parentPanel);

	sf::Rect<int> pos(0,0,450,500);
	challengeList.create(pos, gui, panel);
	challengeList.show();

	leaderPanel = tgui::Panel::create();
	leaderPanel->setPosition(460,0);
	leaderPanel->setSize(500,500);
	leaderPanel->setBackgroundColor(sf::Color(255,255,255,0));
	leaderPanel->hide();
	panel->add(leaderPanel);

	title = gui->themeTG->load("Label");
	title->setPosition(113,0);
	title->setText("Leader board");
	title->setTextSize(33);
	leaderPanel->add(title);

	scrollPanel = tgui::Panel::create();
	scrollPanel->setPosition(50,60);
	scrollPanel->setSize(420,400);
	scrollPanel->setBackgroundColor(sf::Color(255,255,255,0));
	leaderPanel->add(scrollPanel);

	scroll = gui->themeTG->load("Scrollbar");
	scroll->setSize(30, 440);
	scroll->setPosition(470, 60);
	scroll->setMaximum(2);
	scroll->setLowValue(1);
	//scroll->connect("ValueChanged", &ScrollList::listScrolled, this);
	leaderPanel->add(scroll);

	playChallenge = gui->themeTG->load("Button");
	playChallenge->setSize(180, 50);
	playChallenge->setPosition(160, 430);
	playChallenge->setText("Play challenge");
	playChallenge->connect("pressed", &ChallengesUI::play, this);
}

void ChallengesUI::makeList() {
	sf::Uint8 count;
	gui->net.packet >> count;

	challengeList.removeAllItems();

	sf::Uint16 id;
	sf::String name, label;
	for (int i=0; i<count; i++) {
		gui->net.packet >> id >> name >> label;
		challengeList.addItem(name, label, id);
	}
}

void ChallengesUI::makeLeaderboard() {
	leaderPanel->removeAllWidgets();
	leaderPanel->add(title);
	leaderPanel->add(scroll);
	leaderPanel->add(scrollPanel);
	leaderPanel->add(playChallenge);
	leaderPanel->show();
	sf::Uint8 columns;
	gui->net.packet >> selectedId >> columns;
	for (auto&& chall : challengeList.items)
		if (chall.id == selectedId)
			title->setText(chall.name);

	sf::Uint16* width = new sf::Uint16[columns];
	for (int i=0; i<columns; i++)
		gui->net.packet >> width[i];

	sf::String string;
	for (int i=0; i<columns; i++) {
		gui->net.packet >> string;
		tgui::Label::Ptr label = gui->themeTG->load("Label");
		label->setPosition(width[i]+50, 40);
		label->setText(string);
		label->setTextSize(16);
		leaderPanel->add(label);
	}

	scrollPanel->removeAllWidgets();
	sf::Uint8 count;
	gui->net.packet >> count;
	for (sf::Uint16 c=0; c<count; c++) {
		for (sf::Uint8 i=0; i<columns; i++) {
			gui->net.packet >> string;
			tgui::Label::Ptr label = gui->themeTG->load("Label");
			label->setPosition(width[i], c*30+7);
			label->setText(string);
			label->setTextSize(14);
			scrollPanel->add(label);
		}
		tgui::Button::Ptr button = gui->themeTG->load("Button");
		button->setText("View");
		button->setPosition(350, c*30+5);
		button->setSize(45, 20);
		button->connect("pressed", &ChallengesUI::viewReplay, this, c);
		scrollPanel->add(button);
		scrollPanel->show();
	}

	delete[] width;
}

void ChallengesUI::play() {
	gui->net.sendSignal(17, selectedId);
}

void ChallengesUI::show() {
	panel->show();
	leaderPanel->hide();
}

void ChallengesUI::viewReplay(sf::Uint16 slot) {
	gui->net.sendSignal(18, selectedId, slot);
}