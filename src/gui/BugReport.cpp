#include "BugReport.h"
#include "gui.h"
#include "network.h"
#include <stdio.h>
#include <curl/curl.h>
using std::cout;
using std::endl;
using std::to_string;

void BugReport::create(sf::Rect<int> _pos, UI* _gui) {
	gui = _gui;
	pos = _pos;

	tgui::Button::Ptr DbG = gui->themeBB->load("Button");
	DbG->setPosition(320, 565);
	DbG->setSize(120, 32);
	DbG->setText("Bug Report");
	DbG->connect("Pressed", &BugReport::bugReport, this);
	gui->tGui.add(DbG);

	ChildWindow = gui->themeBB->load("ChildWindow");
	ChildWindow->setTitleButtons(static_cast<tgui::ChildWindow::TitleButtons>(3));
	ChildWindow->connect("Minimized", &BugReport::minimize, this);
	ChildWindow->connect("Closed", &BugReport::close, this);
	ChildWindow->setOpacity(0.95);
	ChildWindow->setPosition(500, 40);
	ChildWindow->setSize(400, 475);
	ChildWindow->hide();
	gui->tGui.add(ChildWindow);

	tgui::Label::Ptr WhL = gui->themeBB->load("Label");
	WhL->setPosition(10, 5);
	WhL->setText("What happened?");
	ChildWindow->add(WhL);

	happened = gui->themeBB->load("TextBox");
	happened->setPosition(5, 40);
	happened->setSize(390, 80);
	ChildWindow->add(happened);

	tgui::Label::Ptr WhL2 = gui->themeBB->load("Label");
	WhL2->setPosition(10, 125);
	WhL2->setText("What did you expect to happen?");
	ChildWindow->add(WhL2);

	expected = gui->themeBB->load("TextBox");
	expected->setPosition(5, 160);
	expected->setSize(390, 80);
	ChildWindow->add(expected);

	tgui::Label::Ptr WhL3 = gui->themeBB->load("Label");
	WhL3->setPosition(10, 245);
	WhL3->setText("How can we reproduce this?");
	ChildWindow->add(WhL3);

	reproduce = gui->themeBB->load("TextBox");
	reproduce->setPosition(5, 280);
	reproduce->setSize(390, 80);
	ChildWindow->add(reproduce);

	tgui::Label::Ptr WhL4 = gui->themeBB->load("Label");
	WhL4->setPosition(10, 365);
	WhL4->setText("How can we contact you with questions?");
	ChildWindow->add(WhL4);

	contact = gui->themeBB->load("TextBox");
	contact->setPosition(5, 400);
	contact->setSize(390, 40);
	ChildWindow->add(contact);

	tgui::Button::Ptr send = gui->themeBB->load("Button");
	send->setPosition(170, 445);
	send->setSize(60, 30);
	send->setText("Send");
	send->connect("Pressed", &BugReport::sendReport, this);
	ChildWindow->add(send);
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

	sf::String version = to_string(gui->clientVersion);

	sf::String postfield = "{\"happening\":\"" + shappened + "\",\"supposed\":\"" + sexpected + "\",\"reproduce\":\"" + sreproduce + "\",\"contact\":\"" + scontact + "\",\"version\":\"" + version + "\",\"os\":\"" + os + "\"}";

	cout << gui->net.sendCurlPost("https://bugs.speedblocks.se/bugs", postfield, 0).toAnsiString() << endl;
	ChildWindow->hide();
}

void BugReport::bugReport() {
	ChildWindow->show();
}

void BugReport::close() {
	ChildWindow->hide();
}

void BugReport::minimize() {
	if (ChildWindow->getSize().y == 0)
		ChildWindow->setSize(ChildWindow->getSize().x, 475);
	else
		ChildWindow->setSize(ChildWindow->getSize().x, 0);
}