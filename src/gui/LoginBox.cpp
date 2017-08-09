#include "LoginBox.h"
#include "gui.h"
#include "network.h"
#include "gamePlay.h"
#include "MainMenu.h"
#include "Connecting.h"
#include "OnlineplayUI.h"
#include "AnimatedBackground.h"

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
	panel->add(LiE1);

	tgui::EditBox::Ptr LiE2 = gui->themeTG->load("EditBox");
	LiE2->setPosition(120, 120);
	LiE2->setSize(180, 30);
	LiE2->setPasswordCharacter('*');
	LiE2->connect("ReturnKeyPressed", &LoginBox::login, this, std::bind(&tgui::EditBox::getText, LiE1), std::bind(&tgui::EditBox::getText, LiE2), 0);
	panel->add(LiE2);

	tgui::CheckBox::Ptr remember = gui->themeTG->load("CheckBox");
	remember->setText("Remember me");
	remember->setPosition(120, 160);
	panel->add(remember);

	tgui::Button::Ptr LiB1 = gui->themeTG->load("Button");
	LiB1->setPosition(160, 200);
	LiB1->setSize(100, 40);
	LiB1->setText("Login");
	LiB1->connect("pressed", &LoginBox::login, this, std::bind(&tgui::EditBox::getText, LiE1), std::bind(&tgui::EditBox::getText, LiE2), 0);
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
	LiL4->setPosition(145, 340);
	panel->add(LiL4);

	tgui::Label::Ptr LiL3 = gui->themeTG->load("Label");
	LiL3->setText("Name");
	LiL3->setPosition(53, 373);
	panel->add(LiL3);

	tgui::EditBox::Ptr LiE3 = gui->themeTG->load("EditBox");
	LiE3->setPosition(120, 370);
	LiE3->setSize(180, 30);
	LiE3->connect("ReturnKeyPressed", &LoginBox::login, this, std::bind(&tgui::EditBox::getText, LiE3), "", 1);
	panel->add(LiE3);

	tgui::Button::Ptr LiB3 = gui->themeTG->load("Button");
	LiB3->setPosition(135, 420);
	LiB3->setSize(150, 40);
	LiB3->setText("Login as Guest");
	LiB3->connect("pressed", &LoginBox::login, this, std::bind(&tgui::EditBox::getText, LiE3), "", 1);
	panel->add(LiB3);
}

void LoginBox::login(const sf::String& name, const sf::String& pass, sf::Uint8 guest) {
	if (guest && !name.getSize())
		return;
	gui->mainMenu->hide();
	gui->connectingScreen->show();
	gui->animatedBackground->draw(*gui->window, gui->delayClock.getElapsedTime()); //Update the screen so a block on connect will show the connecting screen
	gui->tGui.draw();
	gui->window->display();
	if (gui->net.connect() == sf::Socket::Done) {
		gui->net.udpSock.unbind();
		gui->net.udpSock.bind(sf::Socket::AnyPort);
		sf::String hash;
		if (!guest) {
			hash = gui->net.sendCurlPost("https://speedblocks.se/secure_auth.php", "name=" + name + "&pass=" + pass, 1);
			sendLogin(hash, guest);
		}
		else {
			sendLogin(name, guest);
			gui->game.field.text.setName(name);
		}
		gui->playonline=true;
		gui->onlineplayUI->updateRoomListTime = gui->delayClock.getElapsedTime() + sf::seconds(5);
		((guiBase*)gui->chatScreen)->show();
	}
	else {
		gui->net.disconnect();
		gui->quickMsg("Could not connect to server");
		gui->connectingScreen->hide();
		gui->mainMenu->show();
	}
}

void LoginBox::open() {
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