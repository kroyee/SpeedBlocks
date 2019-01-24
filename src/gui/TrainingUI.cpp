#include "TrainingUI.h"
#include "GameSignals.h"
#include "Resources.h"

static auto& AmountAI = Signal<void, uint8_t>::get("AmountAI");
static auto& SpeedAI = Signal<void, uint16_t>::get("SpeedAI");
static auto& StartCountDown = Signal<void>::get("StartCountDown");
static auto& SetGameState = Signal<void, GameStates>::get("SetGameState");
static auto& Show = Signal<void, int>::get("Show");

TrainingUI::TrainingUI(sf::Rect<int> _pos, Resources& _res) : GuiBase(_pos, _res) {
    os::Label().pos(171, 27).text("Train against AI").text_size(38).add_to(panel);

    os::Label().pos(218, 90).text("Amount:").text_size(18).add_to(panel);

    amount_text.pos(330, 90).text("0").text_size(18).add_to(panel);

    amount_slider.pos(60, 130).size(500, 16).max(20).connect("ValueChanged", [&](int newVal) { amount_text.text(std::to_string(newVal)); }).add_to(panel);

    os::Label().pos(238, 170).text("Speed:").text_size(18).add_to(panel);

    speed_text.pos(330, 170).text("20 BPM").text_size(18).add_to(panel);

    speed_slider.pos(60, 210).size(500, 16).min(20).max(300).connect("ValueChanged", [&](int newVal) { speed_text.text(std::to_string(newVal) + " BPM"); }).add_to(panel);

    os::Label().pos(204, 250).text("Skill level:").text_size(18).add_to(panel).hide();

    skill_text.pos(330, 250).text("Normal").text_size(18).add_to(panel);

    skill_slider.pos(60, 290).size(500, 16).add_to(panel);

    os::Label().pos(210, 330).text("Playstyle:").text_size(18).add_to(panel).hide();

    playstyle_text.pos(330, 330).text("Standard").text_size(18).add_to(panel);

    playstyle_slider.pos(60, 370).size(500, 16).add_to(panel);

    os::Button().pos(249, 440).size(120, 30).text("Start!").add_to(panel).connect("pressed", [&]() {
        AmountAI(amount_slider->getValue());
        SpeedAI(speed_slider->getValue());
        StartCountDown();
        SetGameState(GameStates::CountDown);
    });

    os::Button().pos(100, 440).size(120, 30).text("Back").add_to(panel).connect("pressed", [&]() {
        hide();
        Show(0);
    });

    os::ApplyProperty<os::Hide>::apply(skill_text, skill_slider, playstyle_text, playstyle_slider);
}
