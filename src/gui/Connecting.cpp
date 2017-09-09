#include "Connecting.h"
#include "gui.h"
#include "LoginBox.h"

void Connecting::create(sf::Rect<int> _pos, UI* _gui) {
	createBase(_pos, _gui);

	label = gui->themeTG->load("Label");
	label->setPosition(130, 290);
	label->setSize(700, 50);
	label->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Center);
	label->setText("Connecting to server...");
	panel->add(label);

	changelog = gui->themeTG->load("TextBox");
	changelog->setPosition(180, 100);
	changelog->setSize(600, 400);
	changelog->hide();
	changelog->disable();
	panel->add(changelog);

	cancel = gui->themeTG->load("Button");
	cancel->setPosition(390, 520);
	cancel->setSize(150, 60);
	cancel->setText("Cancel");
	cancel->hide();
	cancel->connect("pressed", [&](){
		label->setText("Aborting... waiting for thread to return");
		if (!gui->loginBox->t.joinable()) {
			hide();
			((guiBase*)gui->mainMenu)->show();
		}
		else
			gui->loginBox->patcher.quit=true;
	});
	panel->add(cancel);

	apply = gui->themeTG->load("Button");
	apply->setPosition(220, 520);
	apply->setSize(150, 60);
	apply->setText("Apply");
	apply->hide();
	apply->disable();
	apply->connect("pressed", [&](){
		gui->options.saveOptions();
		if (gui->loginBox->patcher.apply()) {
			gui->restart=true;
			gui->window->close();
		}
		else {
			gui->quickMsg("Failed to apply patch");
			hide();
			((guiBase*)gui->mainMenu)->show();
		}
	});
	panel->add(apply);
}