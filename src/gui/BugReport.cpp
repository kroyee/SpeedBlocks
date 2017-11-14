#include "BugReport.h"
#include "network.h"
#include <stdio.h>
#include <curl/curl.h>
using std::cout;
using std::endl;
using std::to_string;

BugReport::BugReport(sf::Rect<int> _pos, Resources& _res, tgui::Panel::Ptr parent) : guiBase(_pos, _res, parent) {
	join=false;

	tgui::Label::Ptr WhL = resources.gfx->themeTG->load("Label");
	WhL->setPosition(10, 5);
	WhL->setText("What happened?");
	panel->add(WhL);

	happened = resources.gfx->themeTG->load("TextBox");
	happened->setPosition(5, 40);
	happened->setSize(550, 80);
	panel->add(happened);

	tgui::Label::Ptr WhL2 = resources.gfx->themeTG->load("Label");
	WhL2->setPosition(10, 125);
	WhL2->setText("What did you expect to happen?");
	panel->add(WhL2);

	expected = resources.gfx->themeTG->load("TextBox");
	expected->setPosition(5, 160);
	expected->setSize(550, 80);
	panel->add(expected);

	tgui::Label::Ptr WhL3 = resources.gfx->themeTG->load("Label");
	WhL3->setPosition(10, 245);
	WhL3->setText("How can we reproduce this?");
	panel->add(WhL3);

	reproduce = resources.gfx->themeTG->load("TextBox");
	reproduce->setPosition(5, 280);
	reproduce->setSize(550, 80);
	panel->add(reproduce);

	tgui::Label::Ptr WhL4 = resources.gfx->themeTG->load("Label");
	WhL4->setPosition(10, 365);
	WhL4->setText("How can we contact you with questions?");
	panel->add(WhL4);

	contact = resources.gfx->themeTG->load("TextBox");
	contact->setPosition(5, 400);
	contact->setSize(550, 40);
	panel->add(contact);

	tgui::Button::Ptr send = resources.gfx->themeTG->load("Button");
	send->setPosition(250, 455);
	send->setSize(60, 30);
	send->setText("Send");
	send->connect("Pressed", &BugReport::sendReport, this);
	panel->add(send);
}

void BugReport::sendReport() {
	sf::String shappened = happened->getText();
	sf::String sexpected = expected->getText();
	sf::String sreproduce = reproduce->getText();
	sf::String scontact = contact->getText();

	#ifdef _WIN32
		sf::String os = "Win";
	#elif __APPLE__
		sf::String os = "Mac";
	#else
		sf::String os = "Linux";
	#endif

	sf::String version = to_string(resources.clientVersion);

	sf::String postfield = "{\"happening\":\"" + shappened + "\",\"supposed\":\"" + sexpected + "\",\"reproduce\":\"" + sreproduce + "\",\"contact\":\"" + scontact + "\",\"version\":\"" + version + "\",\"os\":\"" + os + "\"}";

	t = std::thread([&]() { resources.net->sendCurlPost("https://bugs.speedblocks.se/bugs", postfield, 0); join=true; });

	happened->setText(""); //tguiedit
	expected->setText("");
	reproduce->setText("");
	contact->setText("");
}