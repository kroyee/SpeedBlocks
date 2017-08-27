#include "LoginBox.h"
#include "gui.h"
#include "network.h"
#include "gamePlay.h"
#include "MainMenu.h"
#include "Connecting.h"
#include "OnlineplayUI.h"
#include "AnimatedBackground.h"
#include "machineid.h"
#include <iostream>
using std::cout;
using std::endl;
using std::to_string;

void LoginBox::create(sf::Rect<int> _pos, UI* _gui, tgui::Panel::Ptr parent) {
	createBase(_pos, _gui, parent);

	tgui::Label::Ptr header = gui->themeTG->load("Label");
	header->setTextSize(42);
	header->setText("Play online");
	header->setPosition(20, 0);
	header->setFont(gui->resources.gfx.typewriter);
	panel->add(header);

	tgui::Label::Ptr LiL1 = gui->themeTG->load("Label");
	LiL1->setText("Username");
	LiL1->setPosition(20, 83);
	panel->add(LiL1);

	tgui::Label::Ptr LiL2 = gui->themeTG->load("Label");
	LiL2->setText("Password");
	LiL2->setPosition(23, 123);
	panel->add(LiL2);

	LiE1 = gui->themeTG->load("EditBox");
	LiE1->setPosition(120, 80);
	LiE1->setSize(180, 30);
	LiE1->setText(gui->options.username);
	panel->add(LiE1);

	LiE2 = gui->themeTG->load("EditBox");
	LiE2->setPosition(120, 120);
	LiE2->setSize(180, 30);
	LiE2->setPasswordCharacter('*');
	if (gui->options.rememberme) {
		sf::String boguspass;
		for (int i=0; i<gui->options.pass; i++)
			boguspass+="b";
		LiE2->setText(boguspass);
	}
	LiE2->connect("TextChanged", [&](){ edited=true; });
	LiE2->connect("Focused", [&]() { if (!edited) LiE2->setText(""); });
	LiE1->connect("ReturnKeyPressed", &LoginBox::launchLogin, this, 0);
	LiE2->connect("ReturnKeyPressed", &LoginBox::launchLogin, this, 0);
	panel->add(LiE2);

	tgui::CheckBox::Ptr remember = gui->themeTG->load("CheckBox");
	remember->setText("Remember me");
	remember->setPosition(120, 160);
	if (gui->options.rememberme)
		remember->check();
	remember->connect("Checked", [&](){ gui->options.rememberme=true; });
	remember->connect("Unchecked", [&](){ gui->options.rememberme=false; });
	panel->add(remember);

	tgui::Button::Ptr LiB1 = gui->themeTG->load("Button");
	LiB1->setPosition(160, 200);
	LiB1->setSize(100, 40);
	LiB1->setText("Login");
	LiB1->connect("pressed", &LoginBox::launchLogin, this, 0);
	panel->add(LiB1);

	tgui::Button::Ptr regButton = gui->themeTG->load("Button");
	regButton->setPosition(20, 220);
	regButton->setSize(100, 30);
	regButton->setText("Register");
	regButton->connect("pressed", &LoginBox::regPressed, this);
	panel->add(regButton);

	tgui::Button::Ptr forgotButton = gui->themeTG->load("Button");
	forgotButton->setPosition(20, 260);
	forgotButton->setSize(160, 30);
	forgotButton->setText("Forgot password");
	forgotButton->connect("pressed", &LoginBox::forgotPressed, this);
	panel->add(forgotButton);

	tgui::Label::Ptr LiL4 = gui->themeTG->load("Label");
	LiL4->setText("Play as guest");
	LiL4->setPosition(145, 330);
	panel->add(LiL4);

	tgui::Label::Ptr LiL3 = gui->themeTG->load("Label");
	LiL3->setText("Name");
	LiL3->setPosition(53, 363);
	panel->add(LiL3);

	tgui::EditBox::Ptr LiE3 = gui->themeTG->load("EditBox");
	LiE3->setPosition(120, 360);
	LiE3->setSize(180, 30);
	LiE3->connect("ReturnKeyPressed", &LoginBox::launchLogin, this, 1);
	panel->add(LiE3);

	tgui::Button::Ptr LiB3 = gui->themeTG->load("Button");
	LiB3->setPosition(135, 410);
	LiB3->setSize(150, 40);
	LiB3->setText("Login as Guest");
	LiB3->connect("pressed", &LoginBox::launchLogin, this, 1);
	panel->add(LiB3);
}

void LoginBox::launchLogin(sf::Uint8 guest) {
	gui->mainMenu->hide();
	gui->connectingScreen->show();
	gui->connectingScreen->label->setText("Connecting to server...");
	t = std::thread(&LoginBox::login, this, LiE1->getText(), LiE2->getText(), guest);
}

void LoginBox::login(sf::String name, sf::String pass, sf::Uint8 guest) {
	if (guest && !name.getSize())
		return;
	patcher.status=1;
	if (gui->net.connect() == sf::Socket::Done) {
		gui->net.udpSock.unbind();
		gui->net.udpSock.bind(sf::Socket::AnyPort);
		sf::String hash;
		if (!guest) {
			patcher.status=2;
			if (!edited && gui->options.rememberme)
				hash = gui->net.sendCurlPost("https://speedblocks.se/secure_auth.php", "name=" + name + "&remember=" + gui->options.hash + "&machineid=" + machineid::machineHash(), 1);
			else
				hash = gui->net.sendCurlPost("https://speedblocks.se/secure_auth.php", "name=" + name + "&pass=" + pass + "&machineid=" + machineid::machineHash(), 1);
			if (gui->options.rememberme) {
				if (hash.getSize() < 40)
					cout << hash.toAnsiString() << endl << gui->options.hash.toAnsiString() << endl;
				else
					gui->options.hash = hash.substring(20);
			}
			else
				gui->options.hash = "null";
			patcher.status=3;
			hash = hash.substring(0,20);
			gui->options.username=name;
			gui->options.pass = pass.getSize();
			sendLogin(hash, guest);
		}
		else {
			patcher.status=3;
			sendLogin(name, guest);
			gui->game.field.text.setName(name);
		}
		gui->playonline=true;
		gui->onlineplayUI->updateRoomListTime = gui->delayClock.getElapsedTime() + sf::seconds(5);
		((guiBase*)gui->chatScreen)->show();
	}
	else {
		gui->net.disconnect();
		patcher.status=-1;
	}
}

void LoginBox::checkStatus() {
	if (patcher.status == 2)
		gui->connectingScreen->label->setText("Doing secure auth with auth-server...");
	else if (patcher.status == 3)
		gui->connectingScreen->label->setText("Waiting for auth-response from game-server...");
	else if (patcher.status == -1) {
		t.join();
		gui->quickMsg("Could not connect to server");
		gui->connectingScreen->hide();
		gui->mainMenu->show();
	}
	else if (patcher.status == 4) {
		t.join();
		gui->quickMsg("No new version found");
		gui->connectingScreen->hide();
		gui->mainMenu->show();
	}
	else if (patcher.status == 5) {
		gui->connectingScreen->label->setText("New patch found, " + to_string(patcher.files_downloaded) + " of " + to_string(patcher.files_total) + " files downloaded");
	}
	else if (patcher.status == 6) {
		t.join();
		gui->connectingScreen->label->setText("Applying patch...");
		patcher.apply();
		gui->restart=true;
		gui->window->close();
	}
	else if (patcher.status == -2) {
		t.join();
		gui->quickMsg("Error talking to patch server");
		gui->connectingScreen->hide();
		gui->mainMenu->show();
	}
	else if (patcher.status == -3) {
		t.join();
		gui->quickMsg("Error downloading file");
		gui->connectingScreen->hide();
		gui->mainMenu->show();
	}
	else if (patcher.status == -4) {
		t.join();
		gui->quickMsg("The md5-sum for downloaded file did not match, aborting");
		gui->connectingScreen->hide();
		gui->mainMenu->show();
	}
	else if (patcher.status == -5) {
		t.join();
		gui->quickMsg("Error saving file");
		gui->connectingScreen->hide();
		gui->mainMenu->show();
	}
	patcher.status=0;
}

void LoginBox::show() {
	panel->show();
	LiE1->focus();
}

void LoginBox::sendLogin(const sf::String& hashorname, sf::Uint8 guest) {
	sf::Uint8 packetid = 2;
	gui->net.packet.clear();
	gui->net.packet << packetid << gui->clientVersion << guest << hashorname;
	gui->net.sendTCP();
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