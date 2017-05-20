#include "GameplayUI.h"
#include "gui.h"
#include "network.h"
#include "gamePlay.h"
using std::to_string;

void GameplayUI::create(sf::Rect<int> _pos, UI* _gui) {
	createBase(_pos, _gui);

	unknown = "Unknown";

	InGameTab = gui->themeBB->load("Tab");
	InGameTab->add("GameFields");
	InGameTab->add("Score");
	InGameTab->add("Chat");
	InGameTab->setPosition(465, 0);
	InGameTab->select(1);
	InGameTab->connect("TabSelected", &GameplayUI::igtabSelect, this);
	InGameTab->setOpacity(0.7);
	panel->add(InGameTab);

	GameFields = tgui::Panel::create();
	GameFields->setPosition(465, 40);
	GameFields->setSize(490, 555);
	GameFields->hide();
	GameFields->setBackgroundColor(sf::Color(255,255,255,0));
	panel->add(GameFields);

	Score = tgui::Panel::create();
	Score->setPosition(465,40);
	Score->setSize(490, 555);
	Score->hide();
	Score->setBackgroundColor(sf::Color(200,200,200, 50));
	panel->add(Score);

	tgui::Label::Ptr SbL = gui->themeTG->load("Label");
	SbL->setPosition(5, 5);
	SbL->setText("Name                  Score  Rank  BPM  Cmb  Sent  Adj  SPM  Blocked");
	SbL->setTextSize(16);
	Score->add(SbL);

	Chat = tgui::Panel::create();
	Chat->setPosition(465,40);
	Chat->setSize(490, 555);
	Chat->hide();
	Chat->setBackgroundColor(sf::Color(200,200,200, 50));
	panel->add(Chat);

	ChatTab = gui->themeBB->load("Tab");
	ChatTab->add("Room");
	ChatTab->add("Lobby");
	ChatTab->setTabHeight(30);
	ChatTab->select(0);
	ChatTab->connect("TabSelected", &GameplayUI::chattabSelect, this);
	ChatTab->setOpacity(0.7);
	Chat->add(ChatTab);

	Room = gui->themeBB->load("ChatBox");
	Room->setPosition(5, 35);
	Room->setSize(480, 475);
	Chat->add(Room, "Room");

	Lobby = gui->themeBB->load("ChatBox");
	Lobby->setPosition(5, 35);
	Lobby->setSize(480, 475);
	Lobby->hide();
	Chat->add(Lobby, "Lobby");

	ChatBox = gui->themeTG->load("EditBox");
	ChatBox->setPosition(5, 520);
	ChatBox->setSize(480, 30);
	ChatBox->connect("ReturnKeyPressed", &UI::sendMsg, gui, std::bind(&tgui::Tab::getSelected, ChatTab));
	ChatBox->connect("Focused Unfocused", &UI::chatFocus, gui, std::bind(&tgui::Widget::isFocused, ChatBox));
	Chat->add(ChatBox);
}

void GameplayUI::igtabSelect(const std::string& tab) {
	if (tab == "GameFields") {
		GameFields->show();
		Score->hide();
		Chat->hide();
	}
	else if (tab == "Score") {
		GameFields->hide();
		Score->show();
		Chat->hide();
	}
	else if (tab == "Chat") {
		GameFields->hide();
		Score->hide();
		Chat->show();
		ChatBox->focus();
	}
}

void GameplayUI::chattabSelect(const std::string& tab) {
	for (size_t i = 0; i < privChats.size(); i++)
		privChats[i].chatBox->hide();
	Lobby->hide();
	Room->hide();
	Chat->get(tab)->show();
}

void GameplayUI::scoreRow() {
	sf::Uint8 combo, rank, position;
	sf::Uint16 id, sent, received, blocked, bpm, spm, score;
	float adj;

	gui->net.packet >> id >> combo >> sent >> received >> blocked >> bpm >> spm >> rank >> position >> score >> adj;

	sf::String rounding = to_string((int)adj); //A bit messy-looking way of rounding float to 1 decimal
	rounding += "." + to_string((int)((adj - (int)adj)*10));

	tgui::Label::Ptr label = gui->themeTG->load("Label");
	label->setText(getName(id));
	label->setPosition(5, scoreRows*30+5);
	label->setTextSize(14);
	Score->add(label);

	label = gui->themeTG->load("Label");
	label->setText(to_string(score));
	label->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Right);
	label->setPosition(65, scoreRows*30+5);
	label->setSize(100, 35);
	label->setTextSize(14);
	Score->add(label);

	label = gui->themeTG->load("Label");
	label->setText(to_string(rank));
	label->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Right);
	label->setPosition(110, scoreRows*30+5);
	label->setSize(100, 35);
	label->setTextSize(14);
	Score->add(label);

	label = gui->themeTG->load("Label");
	label->setText(to_string(bpm));
	label->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Right);
	label->setPosition(155, scoreRows*30+5);
	label->setSize(100, 35);
	label->setTextSize(14);
	Score->add(label);

	label = gui->themeTG->load("Label");
	label->setText(to_string(combo));
	label->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Right);
	label->setPosition(200, scoreRows*30+5);
	label->setSize(100, 35);
	label->setTextSize(14);
	Score->add(label);

	label = gui->themeTG->load("Label");
	label->setText(to_string(sent));
	label->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Right);
	label->setPosition(240, scoreRows*30+5);
	label->setSize(100, 35);
	label->setTextSize(14);
	Score->add(label);

	label = gui->themeTG->load("Label");
	label->setText(rounding);
	label->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Right);
	label->setPosition(275, scoreRows*30+5);
	label->setSize(100, 35);
	label->setTextSize(14);
	Score->add(label);

	label = gui->themeTG->load("Label");
	label->setText(to_string(spm));
	label->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Right);
	label->setPosition(315, scoreRows*30+5);
	label->setSize(100, 35);
	label->setTextSize(14);
	Score->add(label);

	label = gui->themeTG->load("Label");
	label->setText(to_string(blocked) + "/" + to_string(received));
	label->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Right);
	label->setPosition(380, scoreRows*30+5);
	label->setSize(100, 35);
	label->setTextSize(14);
	Score->add(label);

	scoreRows++;
}

void GameplayUI::clearScore() {
	Score->removeAllWidgets();

	tgui::Label::Ptr label = gui->themeTG->load("Label");
	label->setPosition(5, 5);
	label->setText("Name                  Score  Rank  BPM  Cmb  Sent  Adj  SPM  Blocked");
	label->setTextSize(16);
	Score->add(label);

	scoreRows=1;
}

const sf::String& GameplayUI::getName(sf::Uint16 id) {
	if (id == gui->myId)
		return gui->game.field.name;
	for (auto&& field : gui->fields)
		if (field.id == id)
			return field.name;
	return unknown;
}