#include "ChallengesGameUI.h"
#include "Signal.h"
#include "gamePlay.h"
using std::to_string;

ChallengesGameUI::ChallengesGameUI(sf::Rect<int> _pos, Resources& _res) : guiBase(_pos, _res), challenge(nullptr) {

	header = resources.gfx->themeTG->load("Label");
	header->setPosition(450,50);
	header->setSize(470,70);
	header->setTextSize(60);
	header->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Center);
	panel->add(header);

	for (int i=0; i<7; i++) {
		label[i] = resources.gfx->themeTG->load("Label");
		label[i]->setPosition(450,90 + i*60);
		label[i]->setText("test");
		label[i]->setSize(200, 40);
		label[i]->setTextSize(20);
		label[i]->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Right);
		label[i]->hide();
		panel->add(label[i]);

		editBox[i] = resources.gfx->themeTG->load("EditBox");
		editBox[i]->setPosition(660,87 + i*60);
		editBox[i]->setSize(200,40);
		editBox[i]->disable();
		editBox[i]->hide();
		panel->add(editBox[i]);
	}

	startChallenge = resources.gfx->themeTG->load("Button");
	startChallenge->setPosition(550,510);
	startChallenge->setSize(200,40);
	startChallenge->setText("Start challenge");
	startChallenge->connect("pressed", [&](){ Signals::Ready(); });
	panel->add(startChallenge);

	specLabel = resources.gfx->themeTG->load("Label");
	specLabel->setPosition(315, 200);
	specLabel->setText("test");
	specLabel->setSize(120, 30);
	specLabel->setTextSize(15);
	specLabel->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Center);
	specLabel->hide();
	panel->add(specLabel);

	specEditBox = resources.gfx->themeTG->load("EditBox");
	specEditBox->setPosition(330,230);
	specEditBox->setSize(90,40);
	specEditBox->disable();
	specEditBox->hide();
	panel->add(specEditBox);

	Signals::HideStartChallengeButton.connect(&ChallengesGameUI::hideStartChallengeButton, this);
	Signals::UpdateChallengesUI.connect([&](GameplayData& data){
		if (challenge != nullptr)
			challenge->update(data);
	});
	Signals::Survivor.connect([&]() -> bool {
		if (panel->isVisible() && challenge->type == Challenges::Survivor)
			return true;
		return false;
	});
}

void ChallengesGameUI::clear() {
	if (challenge != nullptr)
		challenge->clear();
}

sf::String ChallengesGameUI::displayTime(const sf::Time& time) {
	sf::String timeString;
	sf::Uint16 total = time.asSeconds();
	sf::Uint16 count = 0;
	while (total > 60) {
		total-=60;
		count++;
	}
	timeString = to_string(count) + ":";
	if (total < 10)
		timeString += "0";
	timeString += to_string(total) + ":";
	count = (time.asMilliseconds() % 1000) / 10;
	if (count < 10)
		timeString += "0";
	timeString += to_string(count);
	return timeString;
}

void ChallengesGameUI::openChallenge(sf::Uint16 whichPanel) {
	challenge.reset(nullptr);
	if (whichPanel == 20000)
		challenge = std::unique_ptr<BaseChallenge>(new CH_Race(*this));
	else if (whichPanel == 20001)
		challenge = std::unique_ptr<BaseChallenge>(new CH_Cheese(*this));
	else if (whichPanel == 20002)
		challenge = std::unique_ptr<BaseChallenge>(new CH_Survivor(*this));
	show();
}

void ChallengesGameUI::hideStartChallengeButton() {
	startChallenge->hide();
}

/////////////////////////////////////////////////////////////
/////				Challenge base class				/////
/////////////////////////////////////////////////////////////

BaseChallenge::BaseChallenge(ChallengesGameUI &_ref) : ref(_ref), specIndex(LabelCount) {
	ref.startChallenge->show();
	setLabel(0, "Time Elapsed");
	type = Challenges::Base;
}
void BaseChallenge::setLabel(int i, const sf::String& text) {
	if (i >= LabelCount)
		return;
	ref.label[i]->show();
	ref.editBox[i]->show();
	ref.label[i]->setText(text);
}
void BaseChallenge::setSpec(int i) {
	if (i >= LabelCount)
		return;
	ref.specLabel->show();
	ref.specEditBox->show();
	ref.specLabel->setText(ref.label[i]->getText());
	specIndex = i;
}
void BaseChallenge::updateSpec() {
	if (specIndex == LabelCount)
		return;
	ref.specEditBox->setText(ref.editBox[specIndex]->getText());
}
void BaseChallenge::setTime() {
	if (ref.resources.gamestate == GameStates::Replay)
		ref.editBox[0]->setText(ref.displayTime(Signals::GetRecTime()));
	else
		ref.editBox[0]->setText(ref.displayTime(Signals::GetGameTime()));
}
void BaseChallenge::clear() {
	for (int i=0; i<LabelCount; i++)
		ref.editBox[i]->setText("");
	ref.specEditBox->setText("");
}
BaseChallenge::~BaseChallenge() {
	for (int i=0; i<LabelCount; i++) {
		ref.label[i]->hide();
		ref.editBox[i]->hide();
	}
	ref.specLabel->hide();
	ref.specEditBox->hide();
}

/////////////////////////////////////////////////////////////
/////				Challenge classes					/////
/////////////////////////////////////////////////////////////

////////// Race

CH_Race::CH_Race(ChallengesGameUI& ref) : BaseChallenge(ref) {
	type = Challenges::Race;
	setLabel(1, "Lines Remaining");
	setLabel(2, "Blocks used");

	setSpec(1);
}

void CH_Race::update(GameplayData& data) {
	ref.editBox[1]->setText(to_string(40 - data.linesCleared));
	ref.editBox[2]->setText(to_string(data.pieceCount));
	if (data.linesCleared > 39)
		Signals::GameOver(1);

	setTime();
	updateSpec();
}

///////// Cheese

CH_Cheese::CH_Cheese(ChallengesGameUI& ref) : BaseChallenge(ref) {
	type = Challenges::Cheese;
	setLabel(1, "Lines Remaining");
	setLabel(2, "Blocks used");

	setSpec(1);
}

void CH_Cheese::update(GameplayData& data) {
	ref.editBox[1]->setText(to_string(9 - data.garbageCleared));
	ref.editBox[2]->setText(to_string(data.pieceCount));
	if (data.garbageCleared > 8)
		Signals::GameOver(1);

	setTime();
	updateSpec();
}

/////// Survivor

CH_Survivor::CH_Survivor(ChallengesGameUI& ref) : BaseChallenge(ref) {
	type = Challenges::Survivor;
	setLabel(1, "Garbage cleared");
	setLabel(2, "Blocks used");

	lineAdd = sf::seconds(0);
}

void CH_Survivor::update(GameplayData& data) {
	ref.editBox[1]->setText(to_string(data.garbageCleared));
	ref.editBox[2]->setText(to_string(data.pieceCount));

	if (ref.resources.gamestate == GameStates::Game)
		if (Signals::GetGameTime() > lineAdd) {
			lineAdd += sf::seconds(2);
			Signals::PushGarbage();
		}

	setTime();
}

void CH_Survivor::clear() {
	for (int i=0; i<LabelCount; i++)
		ref.editBox[i]->setText("");
	ref.specEditBox->setText("");

	lineAdd = sf::seconds(2);
}