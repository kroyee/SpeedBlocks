#include "Connecting.h"
#include "optionSet.h"
#include "GameSignals.h"
#include "Resources.h"

static auto& Show = Signal<void, int>::get("Show");
static auto& QuickMsg = Signal<void, const std::string&>::get("QuickMsg");
static auto& IsLoginThreadJoinable = Signal<bool>::get("IsLoginThreadJoinable");
static auto& TellPatcherToQuit = Signal<void>::get("TellPatcherToQuit");
static auto& ApplyPatch = Signal<int>::get("ApplyPatch");

Connecting::Connecting(sf::Rect<int> _pos, Resources& _res) : GuiBase(_pos, _res) {

	label = resources.gfx->themeTG->load("Label");
	label->setPosition(130, 290);
	label->setSize(700, 50);
	label->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Center);
	label->setText("Connecting to server...");
	panel->add(label);

	downloadProgress = resources.gfx->themeTG->load("ProgressBar");
	downloadProgress->setPosition(180, 90);
	downloadProgress->setSize(600, 30);
	downloadProgress->setMinimum(0);
	downloadProgress->setMaximum(100);
	downloadProgress->setValue(0);
	downloadProgress->hide();
	panel->add(downloadProgress);

	progressLabel = resources.gfx->themeTG->load("Label");
	progressLabel->setPosition(180, 95);
	progressLabel->setSize(600, 30);
	progressLabel->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Center);
	progressLabel->hide();
	panel->add(progressLabel);

	changelog = resources.gfx->themeTG->load("TextBox");
	changelog->setPosition(180, 130);
	changelog->setSize(600, 370);
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
		int result = ApplyPatch();
		if (result == 2)
			resources.window.close();
		else if (result == 1) {
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

void Connecting::setDownloadProgress(int64_t total, int64_t so_far) {
	progressLabel->setText(std::to_string(so_far) + " / " + std::to_string(total));
	int val = (so_far / static_cast<double>(total)) * 100;
	downloadProgress->setValue(val);
}
