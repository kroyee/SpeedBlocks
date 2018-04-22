#include "ChallengesGameUI.h"
#include "GameSignals.h"
#include "Resources.h"
using std::to_string;

static auto& Ready = Signal<void>::get("Ready");
static auto& GetRecTime = Signal<sf::Time>::get("GetRecTime");
static auto& GetGameTime = Signal<sf::Time>::get("GetGameTime");
static auto& GameOver = Signal<void, int>::get("GameOver");
static auto& GameAddDelay = Signal<void, int>::get("GameAddDelay");
static auto& PushGarbage = Signal<void>::get("PushGarbage");

ChallengesGameUI::ChallengesGameUI(sf::Rect<int> _pos, Resources& _res) : GuiBase(_pos, _res), challenge(nullptr) {

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
	startChallenge->connect("pressed", [&](){ Ready(); });
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

	connectSignal("HideStartChallengeButton", &ChallengesGameUI::hideStartChallengeButton, this);
	connectSignal("UpdateChallengesUI", [&](GameplayData& data){
		if (challenge != nullptr) {
			challenge->setTime();
			challenge->update(data);
		}
	});
	connectSignal("Survivor", [&]() -> bool {
		if (panel->isVisible() && challenge->type == Challenges::Survivor)
			return true;
		return false;
	});
	connectSignal("Cheese30L", [&]() -> bool {
		if (panel->isVisible() && challenge->type == Challenges::Cheese30L)
			return true;
		return false;
	});
}

void ChallengesGameUI::clear() {
	if (challenge != nullptr)
		challenge->clear();
}

std::string ChallengesGameUI::displayTime(const sf::Time& time) {
	std::string timeString;
	uint16_t total = time.asSeconds();
	uint16_t count = 0;
	while (total >= 60) {
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

void ChallengesGameUI::openChallenge(uint16_t whichPanel) {
	challenge.reset(nullptr);
	if (whichPanel == 20000)
		challenge = std::unique_ptr<BaseChallenge>(new CH_Race(*this));
	else if (whichPanel == 20001)
		challenge = std::unique_ptr<BaseChallenge>(new CH_Cheese(*this));
	else if (whichPanel == 20002)
		challenge = std::unique_ptr<BaseChallenge>(new CH_Survivor(*this));
	else if (whichPanel == 20003)
		challenge = std::unique_ptr<BaseChallenge>(new CH_Cheese30L(*this));
	show();
}

void ChallengesGameUI::hideStartChallengeButton() {
	startChallenge->hide();
}

void ChallengesGameUI::hide() {
	challenge.reset(nullptr);
	panel->hide();
}

/////////////////////////////////////////////////////////////
/////				Challenge base class				/////
/////////////////////////////////////////////////////////////

BaseChallenge::BaseChallenge(ChallengesGameUI &_ref) : ref(_ref), specIndex(LabelCount) {
	ref.startChallenge->show();
	setLabel(0, "Time Elapsed");
	type = Challenges::Base;
}
void BaseChallenge::setLabel(int i, const std::string& text) {
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
	if (i == 0)
		ref.specEditBox->setTextSize(18);
	else
		ref.specEditBox->setTextSize(27);
}
void BaseChallenge::updateSpec() {
	if (specIndex == LabelCount)
		return;
	ref.specEditBox->setText(ref.editBox[specIndex]->getText());
}
void BaseChallenge::setTime() {
	if (ref.resources.gamestate == GameStates::Replay)
		ref.editBox[0]->setText(ref.displayTime(GetRecTime()));
	else
		ref.editBox[0]->setText(ref.displayTime(GetGameTime()));
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

	updateSpec();

	if (data.linesCleared > 39)
		GameOver(1);
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

	updateSpec();

	if (data.garbageCleared > 8)
		GameOver(1);
}

/////// Survivor

CH_Survivor::CH_Survivor(ChallengesGameUI& ref) : BaseChallenge(ref) {
	type = Challenges::Survivor;
	setLabel(1, "Garbage cleared");
	setLabel(2, "Blocks used");

	setSpec(0);

	lineAdd = sf::seconds(0);
	GameAddDelay(2000);
}

CH_Survivor::~CH_Survivor() {
	GameAddDelay(450);
}

void CH_Survivor::update(GameplayData& data) {
	ref.editBox[1]->setText(to_string(data.garbageCleared));
	ref.editBox[2]->setText(to_string(data.pieceCount));

	if (ref.resources.gamestate == GameStates::Game)
		if (GetGameTime() > lineAdd) {
			lineAdd += sf::seconds(2);
			PushGarbage();
		}

	updateSpec();
}

void CH_Survivor::clear() {
	for (int i=0; i<LabelCount; i++)
		ref.editBox[i]->setText("");
	ref.specEditBox->setText("");

	lineAdd = sf::seconds(2);
}

///////// Cheese 40L

CH_Cheese30L::CH_Cheese30L(ChallengesGameUI& ref) : BaseChallenge(ref) {
	linesAdded=6;
	type = Challenges::Cheese30L;
	setLabel(1, "Lines Remaining");
	setLabel(2, "Blocks used");

	setSpec(1);
}

CH_Cheese30L::~CH_Cheese30L() {}

void CH_Cheese30L::update(GameplayData& data) {
	ref.editBox[1]->setText(to_string(30 - data.garbageCleared));
	ref.editBox[2]->setText(to_string(data.pieceCount));

	if (ref.resources.gamestate == GameStates::Game) {
		if (data.garbageCleared > 29)
			GameOver(1);
		while (linesAdded < 30 && linesAdded - data.garbageCleared < 6) {
			PushGarbage();
			linesAdded++;
		}
	}

	updateSpec();
}

void CH_Cheese30L::clear() {
	for (int i=0; i<LabelCount; i++)
		ref.editBox[i]->setText("");
	ref.specEditBox->setText("");

	linesAdded=6;
}
