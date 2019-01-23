#include "Connecting.h"
#include "GameSignals.h"
#include "Options.h"
#include "Resources.h"

static auto& Show = Signal<void, int>::get("Show");
static auto& QuickMsg = Signal<void, const std::string&>::get("QuickMsg");
static auto& IsLoginThreadJoinable = Signal<bool>::get("IsLoginThreadJoinable");
static auto& TellPatcherToQuit = Signal<void>::get("TellPatcherToQuit");
static auto& ApplyPatch = Signal<int>::get("ApplyPatch");

Connecting::Connecting(sf::Rect<int> _pos, Resources& _res) : GuiBase(_pos, _res) {
    label.pos(130, 290).size(700, 50).center().text("Connecting to server...").add_to(panel);

    downloadProgress.pos(180, 90).size(600, 30).hide().min(0).max(100).set(0).add_to(panel).title_top();

    changelog.pos(180, 130).size(600, 370).hide().disable().add_to(panel);

    cancel.pos(390, 520).size(150, 60).text("Cancel").hide().add_to(panel).connect("pressed", [&]() {
        label.text("Aborting... waiting for thread to return");
        if (IsLoginThreadJoinable()) {
            hide();
            Show(0);
        } else
            TellPatcherToQuit();
    });

    apply.pos(220, 520).size(150, 60).text("Apply").hide().disable().add_to(panel).connect("pressed", [&]() {
        Options::save();
        int result = ApplyPatch();
        if (result == 2)
            resources.window.close();
        else if (result == 1) {
            resources.restart = true;
            resources.window.close();
        } else {
            QuickMsg("Failed to apply patch");
            hide();
            Show(0);
        }
    });

    connectSignal("SetConnectingText", &Connecting::setText, this);
}

void Connecting::setText(const std::string& text) { label.text(text); }

void Connecting::setDownloadProgress(int64_t total, int64_t so_far) {
    downloadProgress.title(std::to_string(so_far) + " / " + std::to_string(total));
    int val = (so_far / static_cast<double>(total)) * 100;
    downloadProgress.set(val);
}
