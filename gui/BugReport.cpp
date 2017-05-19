#include "BugReport.h"
#include "gui.h"
#include <stdio.h>
#include <curl/curl.h>
using std::cout;
using std::endl;

void BugReport::create(sf::Rect<int> _pos, UI* _ui) {
	ui = _ui;
	pos = _pos;

	tgui::Button::Ptr DbG = ui->themeBB->load("Button");
	DbG->setPosition(320, 565);
	DbG->setSize(120, 32);
	DbG->setText("Bug Report");
	DbG->connect("Pressed", &BugReport::bugReport, this);
	ui->gui.add(DbG);

	ChildWindow = ui->themeBB->load("ChildWindow");
	ChildWindow->setTitleButtons(static_cast<tgui::ChildWindow::TitleButtons>(3));
	ChildWindow->connect("Minimized", &BugReport::minimize, this);
	ChildWindow->connect("Closed", &BugReport::close, this);
	ChildWindow->setOpacity(0.95);
	ChildWindow->setPosition(500, 40);
	ChildWindow->setSize(400, 475);
	ChildWindow->hide();
	ui->gui.add(ChildWindow);

	tgui::Label::Ptr WhL = ui->themeBB->load("Label");
	WhL->setPosition(10, 5);
	WhL->setText("What happened?");
	ChildWindow->add(WhL);

	happened = ui->themeBB->load("TextBox");
	happened->setPosition(5, 40);
	happened->setSize(390, 80);
	ChildWindow->add(happened);

	tgui::Label::Ptr WhL2 = ui->themeBB->load("Label");
	WhL2->setPosition(10, 125);
	WhL2->setText("What did you expect to happen?");
	ChildWindow->add(WhL2);

	expected = ui->themeBB->load("TextBox");
	expected->setPosition(5, 160);
	expected->setSize(390, 80);
	ChildWindow->add(expected);

	tgui::Label::Ptr WhL3 = ui->themeBB->load("Label");
	WhL3->setPosition(10, 245);
	WhL3->setText("How can we reproduce this?");
	ChildWindow->add(WhL3);

	reproduce = ui->themeBB->load("TextBox");
	reproduce->setPosition(5, 280);
	reproduce->setSize(390, 80);
	ChildWindow->add(reproduce);

	tgui::Label::Ptr WhL4 = ui->themeBB->load("Label");
	WhL4->setPosition(10, 365);
	WhL4->setText("How can we contact you with questions?");
	ChildWindow->add(WhL4);

	contact = ui->themeBB->load("TextBox");
	contact->setPosition(5, 400);
	contact->setSize(390, 40);
	ChildWindow->add(contact);

	tgui::Button::Ptr send = ui->themeBB->load("Button");
	send->setPosition(170, 445);
	send->setSize(60, 30);
	send->setText("Send");
	send->connect("Pressed", &BugReport::sendReport, this);
	ChildWindow->add(send);
}

void BugReport::sendReport() {
	CURL *curl;
	CURLcode res;

	sf::String shappened = happened->getText();
	sf::String sexpected = expected->getText();
	sf::String sreproduce = reproduce->getText();
	sf::String scontact = contact->getText();

	sf::String postfield = "{\"happening\":\"" + shappened + "\",\"supposed\":\"" + sexpected + "\",\"reproduce\":\"" + sreproduce + "\",\"contact\":\"" + scontact + "\"}";

	char * cstr = new char [postfield.getSize()+1];
	std::strcpy (cstr, postfield.toAnsiString().c_str());

	/* In windows, this will init the winsock stuff */ 
	curl_global_init(CURL_GLOBAL_ALL);

	/* get a curl handle */ 
	curl = curl_easy_init();
	if(curl) {
		/* First set the URL that is about to receive our POST. This URL can
		just as well be a https:// URL if that is what should receive the
		data. */ 
		struct curl_slist *headers = NULL;

		curl_easy_setopt(curl, CURLOPT_URL, "https://speedblocks.spdns.org/bugs");
		headers = curl_slist_append(headers, "Content-Type: application/json");
		headers = curl_slist_append(headers, "Cache-Control: no-cache");
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
		/* Now specify the POST data */ 
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, cstr);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);
		/* Perform the request, res will get the return code */ 
		res = curl_easy_perform(curl);
		/* Check for errors */ 
		if(res != CURLE_OK)
			fprintf(stderr, "curl_easy_perform() failed: %s\n",
		curl_easy_strerror(res));

		curl_slist_free_all(headers);

		/* always cleanup */ 
		curl_easy_cleanup(curl);
	}
	else
		cout << "Curl failed to load" << endl;
	curl_global_cleanup();
	ChildWindow->hide();
	delete[] cstr;
	cout << endl;
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