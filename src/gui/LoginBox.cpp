#include "LoginBox.h"
#include "machineid.h"
#include "optionSet.h"
#include "network.h"
#include "Signal.h"
#include <iostream>
using std::cout;
using std::endl;
using std::to_string;

LoginBox::LoginBox(sf::Rect<int> _pos, Resources& _res, tgui::Panel::Ptr parent) :
guiBase(_pos, _res, parent), connectingScreen(sf::Rect<int>(0,0,960,600), resources) {

	tgui::Label::Ptr header = resources.gfx->themeTG->load("Label");
	header->setTextSize(42);
	header->setText("Play online");
	header->setPosition(20, 0);
	header->setFont(resources.gfx->typewriter);
	panel->add(header);

	tgui::Label::Ptr LiL1 = resources.gfx->themeTG->load("Label");
	LiL1->setText("Username");
	LiL1->setPosition(20, 83);
	panel->add(LiL1);

	tgui::Label::Ptr LiL2 = resources.gfx->themeTG->load("Label");
	LiL2->setText("Password");
	LiL2->setPosition(23, 123);
	panel->add(LiL2);

	LiE1 = resources.gfx->themeTG->load("EditBox");
	LiE1->setPosition(120, 80);
	LiE1->setSize(180, 30);
	LiE1->setText(resources.options->username);
	panel->add(LiE1);

	LiE2 = resources.gfx->themeTG->load("EditBox");
	LiE2->setPosition(120, 120);
	LiE2->setSize(180, 30);
	LiE2->setPasswordCharacter('*');
	if (resources.options->rememberme) {
		sf::String boguspass;
		for (int i=0; i<resources.options->pass; i++)
			boguspass+="b";
		LiE2->setText(boguspass);
	}
	LiE2->connect("TextChanged", [&](){ edited=true; });
	LiE2->connect("Focused", [&]() { if (!edited) LiE2->setText(""); });
	LiE1->connect("ReturnKeyPressed", &LoginBox::launchLogin, this, 0);
	LiE2->connect("ReturnKeyPressed", &LoginBox::launchLogin, this, 0);
	panel->add(LiE2);

	tgui::CheckBox::Ptr remember = resources.gfx->themeTG->load("CheckBox");
	remember->setText("Remember me");
	remember->setPosition(120, 160);
	if (resources.options->rememberme)
		remember->check();
	remember->connect("Checked", [&](){ resources.options->rememberme=true; });
	remember->connect("Unchecked", [&](){ resources.options->rememberme=false; });
	panel->add(remember);

	tgui::Button::Ptr LiB1 = resources.gfx->themeTG->load("Button");
	LiB1->setPosition(160, 200);
	LiB1->setSize(100, 40);
	LiB1->setText("Login");
	LiB1->connect("pressed", &LoginBox::launchLogin, this, 0);
	panel->add(LiB1);

	tgui::Button::Ptr regButton = resources.gfx->themeTG->load("Button");
	regButton->setPosition(20, 220);
	regButton->setSize(100, 30);
	regButton->setText("Register");
	regButton->connect("pressed", &LoginBox::regPressed, this);
	panel->add(regButton);

	tgui::Button::Ptr forgotButton = resources.gfx->themeTG->load("Button");
	forgotButton->setPosition(20, 260);
	forgotButton->setSize(160, 30);
	forgotButton->setText("Forgot password");
	forgotButton->connect("pressed", &LoginBox::forgotPressed, this);
	panel->add(forgotButton);

	tgui::Label::Ptr LiL4 = resources.gfx->themeTG->load("Label");
	LiL4->setText("Play as guest");
	LiL4->setPosition(145, 330);
	panel->add(LiL4);

	tgui::Label::Ptr LiL3 = resources.gfx->themeTG->load("Label");
	LiL3->setText("Name");
	LiL3->setPosition(53, 363);
	panel->add(LiL3);

	tgui::EditBox::Ptr LiE3 = resources.gfx->themeTG->load("EditBox");
	LiE3->setPosition(120, 360);
	LiE3->setSize(180, 30);
	LiE3->connect("ReturnKeyPressed", &LoginBox::launchLogin, this, 1);
	panel->add(LiE3);

	tgui::Button::Ptr LiB3 = resources.gfx->themeTG->load("Button");
	LiB3->setPosition(135, 410);
	LiB3->setSize(150, 40);
	LiB3->setText("Login as Guest");
	LiB3->connect("pressed", &LoginBox::launchLogin, this, 1);
	panel->add(LiB3);

	Signals::IsLoginThreadJoinable.connect(&LoginBox::isThreadJoinable, this);
	Signals::TellPatcherToQuit.connect(&LoginBox::tellPatcherToQuit, this);
}

void LoginBox::launchLogin(sf::Uint8 guest) {
	Signals::Hide(0);
	connectingScreen.show();
	connectingScreen.label->setText("Connecting to server...");
	t = std::thread(&LoginBox::login, this, LiE1->getText(), LiE2->getText(), guest);
}

void LoginBox::login(sf::String name, sf::String pass, sf::Uint8 guest) {
	if (guest && !name.getSize())
		return;
	patcher.status=1;
	if (resources.net->connect() == sf::Socket::Done) {
		resources.net->udpSock.unbind();
		resources.net->udpSock.bind(sf::Socket::AnyPort);
		sf::String hash;
		if (!guest) {
			patcher.status=2;
			if (!edited && resources.options->rememberme)
				hash = resources.net->sendCurlPost("https://speedblocks.se/secure_auth.php", "name=" + name + "&remember=" + resources.options->hash + "&machineid=" + machineid::machineHash(), 1);
			else
				hash = resources.net->sendCurlPost("https://speedblocks.se/secure_auth.php", "name=" + name + "&pass=" + pass + "&machineid=" + machineid::machineHash(), 1);
			if (resources.options->rememberme) {
				if (hash.getSize() < 40)
					cout << hash.toAnsiString() << endl << resources.options->hash.toAnsiString() << endl;
				else
					resources.options->hash = hash.substring(20);
			}
			else
				resources.options->hash = "null";
			patcher.status=3;
			hash = hash.substring(0,20);
			resources.options->username=name;
			resources.options->pass = pass.getSize();
			sendLogin(hash, guest);
		}
		else {
			patcher.status=3;
			sendLogin(name, guest);
			Signals::SetName(name);
			resources.name = name;
		}
		resources.playonline=true;
		Signals::SetRoomListTime();
		Signals::Show(11);
	}
	else {
		resources.net->disconnect(0);
		patcher.status=-1;
	}
}

void LoginBox::checkStatus() {
	if (patcher.status == 2) {
		connectingScreen.label->setText("Doing secure auth with auth-server...");
		connectingScreen.label->setPosition(130, 290);
		connectingScreen.changelog->hide();
		connectingScreen.cancel->hide();
		connectingScreen.apply->hide();
		connectingScreen.apply->disable();
	}
	else if (patcher.status == 3)
		connectingScreen.label->setText("Waiting for auth-response from game-server...");
	else if (patcher.status == -1) {
		t.join();
		Signals::QuickMsg("Could not connect to server");
		connectingScreen.hide();
		Signals::Show(0);
	}
	else if (patcher.status == 4) {
		t.join();
		Signals::QuickMsg("No new version found");
		connectingScreen.hide();
		Signals::Show(0);
	}
	else if (patcher.status == 5) {
		connectingScreen.label->setText("New patch found, " + to_string(patcher.files_downloaded) + " of " + to_string(patcher.files_total) + " files downloaded");
		connectingScreen.label->setPosition(130, 40);
		connectingScreen.changelog->setText(patcher.changelog);
		connectingScreen.changelog->show();
		connectingScreen.cancel->show();
	}
	else if (patcher.status == 6) {
		t.join();
		connectingScreen.label->setText("Patch is ready!");
		connectingScreen.apply->show();
		connectingScreen.apply->enable();
	}
	else if (patcher.status == -2) {
		t.join();
		Signals::QuickMsg("Error talking to patch server");
		connectingScreen.hide();
		Signals::Show(0);
	}
	else if (patcher.status == -3) {
		t.join();
		Signals::QuickMsg("Error downloading file");
		connectingScreen.hide();
		Signals::Show(0);
	}
	else if (patcher.status == -4) {
		t.join();
		Signals::QuickMsg("The md5-sum for downloaded file did not match, aborting");
		connectingScreen.hide();
		Signals::Show(0);
	}
	else if (patcher.status == -5) {
		t.join();
		Signals::QuickMsg("Error saving file");
		connectingScreen.hide();
		Signals::Show(0);
	}
	else if (patcher.status == -6) {
		t.join();
		connectingScreen.hide();
		Signals::Show(0);
	}
	patcher.status=0;
}

void LoginBox::show() {
	panel->show();
	LiE1->focus();
}

void LoginBox::sendLogin(const sf::String& hashorname, sf::Uint8 guest) {
	sf::Packet packet;
	packet << (sf::Uint8)2 << resources.clientVersion << guest << hashorname;
	Signals::SendPacket(packet);
}

void LoginBox::regPressed() {
	#ifdef _WIN32
		system("start https://speedblocks.se/forum/ucp.php?mode=register");
	#elif __APPLE__
		system("open https://speedblocks.se/forum/ucp.php?mode=register");
	#else
		system("xdg-open https://speedblocks.se/forum/ucp.php?mode=register");
	#endif
}

void LoginBox::forgotPressed() {
	#ifdef _WIN32
		system("start https://speedblocks.se/forum/ucp.php?mode=sendpassword");
	#elif __APPLE__
		system("open https://speedblocks.se/forum/ucp.php?mode=sendpassword");
	#else
		system("xdg-open https://speedblocks.se/forum/ucp.php?mode=sendpassword");
	#endif
}

bool LoginBox::isThreadJoinable() {
	return t.joinable();
}

void LoginBox::tellPatcherToQuit() {
	patcher.quit = true;
}