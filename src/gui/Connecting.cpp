#include "Connecting.h"
#include "optionSet.h"
#include "GameSignals.h"

static auto& Show = Signal<void, int>::get("Show");
static auto& QuickMsg = Signal<void, const std::string&>::get("QuickMsg");
static auto& IsLoginThreadJoinable = Signal<bool>::get("IsLoginThreadJoinable");
static auto& TellPatcherToQuit = Signal<void>::get("TellPatcherToQuit");
static auto& ApplyPatch = Signal<bool>::get("ApplyPatch");

Connecting::Connecting(sf::Rect<int> _pos, Resources& _res) : guiBase(_pos, _res) {

	label = resources.gfx->themeTG->load("Label");
	label->setPosition(130, 290);
	label->setSize(700, 50);
	label->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Center);
	label->setText("Connecting to server...");
	panel->add(label);

	changelog = resources.gfx->themeTG->load("TextBox");
	changelog->setPosition(180, 100);
	changelog->setSize(600, 400);
	changelog->hide();
	changelog->disable();
	panel->add(changelog);

	cancel = resources.gfx->themeTG->load("Button");
	cancel->setPosition(390, 520);
	cancel->setSize(150, 60);
	cancel->setText("Cancel");
	cancel->hide();
	cancel->connect("pressed", [&](){
		label->setText("Aborting... waiting for thread to return");
		if (IsLoginThreadJoinable()) {
			hide();
			Show(0);
		}
		else
			TellPatcherToQuit();
	});
	panel->add(cancel);

	apply = resources.gfx->themeTG->load("Button");
	apply->setPosition(220, 520);
	apply->setSize(150, 60);
	apply->setText("Apply");
	apply->hide();
	apply->disable();
	apply->connect("pressed", [&](){
		resources.options->saveOptions();
		if (ApplyPatch()) {
			resources.restart=true;
			resources.window.close();
		}
		else {
			QuickMsg("Failed to apply patch");
			hide();
			Show(0);
		}
	});
	panel->add(apply);

	connectSignal("SetConnectingText", &Connecting::setText, this);
}

void Connecting::setText(const std::string& text) {
	label->setText(text);
}