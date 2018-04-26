#include "TrainingUI.h"
#include "GameSignals.h"
#include "Resources.h"

static auto& AmountAI = Signal<void, uint8_t>::get("AmountAI");
static auto& SpeedAI = Signal<void, uint16_t>::get("SpeedAI");
static auto& StartCountDown = Signal<void>::get("StartCountDown");
static auto& SetGameState = Signal<void, GameStates>::get("SetGameState");
static auto& ShowGameFields = Signal<void>::get("ShowGameFields");
static auto& Show = Signal<void, int>::get("Show");

TrainingUI::TrainingUI(sf::Rect<int> _pos, Resources& _res) : GuiBase(_pos, _res) {
	tgui::Label::Ptr widget0 = resources.gfx->load("Label");
	widget0->setPosition(171,27);
	widget0->setText("Train against AI");
	widget0->setTextSize(38);
	panel->add(widget0);

	tgui::Label::Ptr widget1 = resources.gfx->load("Label");
	widget1->setPosition(218,90);
	widget1->setText("Amount:");
	widget1->setTextSize(18);
	panel->add(widget1);

	amount_text = resources.gfx->load("Label");
	amount_text->setPosition(330,90);
	amount_text->setText("0");
	amount_text->setTextSize(18);
	panel->add(amount_text);

	amount_slider = resources.gfx->load("Slider");
	amount_slider->setPosition(60,130);
	amount_slider->setSize(500,16);
	amount_slider->setMaximum(20);
	amount_slider->connect("ValueChanged", [&](int newVal){ amount_text->setText(std::to_string(newVal)); });
	panel->add(amount_slider);

	tgui::Label::Ptr widget2 = resources.gfx->load("Label");
	widget2->setPosition(238,170);
	widget2->setText("Speed:");
	widget2->setTextSize(18);
	panel->add(widget2);

	speed_text = resources.gfx->load("Label");
	speed_text->setPosition(330,170);
	speed_text->setText("20 BPM");
	speed_text->setTextSize(18);
	panel->add(speed_text);

	speed_slider = resources.gfx->load("Slider");
	speed_slider->setPosition(60,210);
	speed_slider->setSize(500,16);
	speed_slider->setMinimum(20);
	speed_slider->setMaximum(300);
	speed_slider->connect("ValueChanged", [&](int newVal){ speed_text->setText(std::to_string(newVal) + " BPM"); });
	panel->add(speed_slider);

	tgui::Label::Ptr widget3 = resources.gfx->load("Label");
	widget3->setPosition(204,250);
	widget3->setText("Skill level:");
	widget3->setTextSize(18);
	panel->add(widget3);

	skill_text = resources.gfx->load("Label");
	skill_text->setPosition(330,250);
	skill_text->setText("Normal");
	skill_text->setTextSize(18);
	panel->add(skill_text);

	skill_slider = resources.gfx->load("Slider");
	skill_slider->setPosition(60,290);
	skill_slider->setSize(500,16);
	panel->add(skill_slider);

	tgui::Label::Ptr widget4 = resources.gfx->load("Label");
	widget4->setPosition(210,330);
	widget4->setText("Playstyle:");
	widget4->setTextSize(18);
	panel->add(widget4);

	playstyle_text = resources.gfx->load("Label");
	playstyle_text->setPosition(330,330);
	playstyle_text->setText("Standard");
	playstyle_text->setTextSize(18);
	panel->add(playstyle_text);

	playstyle_slider = resources.gfx->load("Slider");
	playstyle_slider->setPosition(60,370);
	playstyle_slider->setSize(500,16);
	panel->add(playstyle_slider);

	tgui::Button::Ptr widget5 = resources.gfx->load("Button");
	widget5->setPosition(249,440);
	widget5->setSize(120,30);
	widget5->setText("Start!");
	widget5->connect("pressed", [&](){
		AmountAI(amount_slider->getValue());
		SpeedAI(speed_slider->getValue());
		StartCountDown();
		SetGameState(GameStates::CountDown);
		ShowGameFields();
	});
	panel->add(widget5);

	tgui::Button::Ptr widget6 = resources.gfx->load("Button");
	widget6->setPosition(100,440);
	widget6->setSize(120,30);
	widget6->setText("Back");
	widget6->connect("pressed", [&](){
		hide();
		Show(0);
	});
	panel->add(widget6);

	widget3->hide();
	skill_text->hide();
	skill_slider->hide();
	widget4->hide();
	playstyle_text->hide();
	playstyle_slider->hide();
}
