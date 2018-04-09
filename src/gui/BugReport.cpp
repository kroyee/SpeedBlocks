#include "BugReport.h"
#include "network.h"
#include <stdio.h>
#include <curl/curl.h>
using std::cout;
using std::endl;
using std::to_string;

BugReport::BugReport(sf::Rect<int> _pos, Resources& _res, tgui::Panel::Ptr parent) : guiBase(_pos, _res, parent) {
	join=false;

	loadWidget("Label", {10,5}, "What happened?");

	happened = loadWidget("TextBox", {5, 40, 550, 80});

	loadWidget("Label", {10,125}, "What did you expect to happen?");

	expected = loadWidget("TextBox", {5, 160, 550, 80});

	loadWidget("Label", {10,245}, "How can we reproduce this?");

	reproduce = loadWidget("TextBox", {5, 280, 550, 80});

	loadWidget("Label", {10,365}, "How can we contact you with questions?");

	contact = loadWidget("TextBox", {5, 400, 550, 80});

	tgui::Button::Ptr send = loadWidget("Button", {250, 495, 60, 30}, "Send");
	send->connect("Pressed", &BugReport::sendReport, this);
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

	happened->setText("");
	expected->setText("");
	reproduce->setText("");
	contact->setText("");
}