#include "LoginBox.h"
#include "gui.h"
#include "network.h"
#include "gamePlay.h"
#include "MainMenu.h"
#include "Connecting.h"
#include "OnlineplayUI.h"

void LoginBox::create(sf::Rect<int> _pos, UI* _gui) {
	createBase(_pos, _gui);

	panel->setBackgroundColor(sf::Color(50,50,50,200));

	tgui::Label::Ptr LiL1 = gui->themeTG->load("Label");
	LiL1->setText("Username");
	LiL1->setPosition(20, 10);
	panel->add(LiL1);

	tgui::Label::Ptr LiL2 = gui->themeTG->load("Label");
	LiL2->setText("Password");
	LiL2->setPosition(220, 10);
	panel->add(LiL2);

	LiE1 = gui->themeTG->load("EditBox");
	LiE1->setPosition(10, 40);
	LiE1->setSize(180, 30);
	LiE1->connect("Focused Unfocused", &UI::chatFocus, gui, std::bind(&tgui::Widget::isFocused, LiE1));
	panel->add(LiE1);

	tgui::EditBox::Ptr LiE2 = gui->themeTG->load("EditBox");
	LiE2->setPosition(210, 40);
	LiE2->setSize(180, 30);
	LiE2->setPasswordCharacter('*');
	LiE2->connect("ReturnKeyPressed", &LoginBox::login, this, std::bind(&tgui::EditBox::getText, LiE1), std::bind(&tgui::EditBox::getText, LiE2), 0);
	LiE2->connect("Focused Unfocused", &UI::chatFocus, gui, std::bind(&tgui::Widget::isFocused, LiE2));
	panel->add(LiE2);

	tgui::Button::Ptr LiB1 = gui->themeBB->load("Button");
	LiB1->setPosition(50, 80);
	LiB1->setSize(100, 40);
	LiB1->setOpacity(0.7);
	LiB1->setText("Login");
	LiB1->connect("pressed", &LoginBox::login, this, std::bind(&tgui::EditBox::getText, LiE1), std::bind(&tgui::EditBox::getText, LiE2), 0);
	panel->add(LiB1);

	tgui::Button::Ptr LiB2 = gui->themeBB->load("Button");
	LiB2->setPosition(250, 80);
	LiB2->setSize(100, 40);
	LiB2->setOpacity(0.7);
	LiB2->setText("Cancel");
	LiB2->connect("Pressed", &LoginBox::closeLogin, this);
	panel->add(LiB2);

	tgui::Label::Ptr regLabel = gui->themeTG->load("Label");
	regLabel->setPosition(40, 140);
	regLabel->setText("register at ");
	panel->add(regLabel);

	tgui::Button::Ptr regButton = gui->themeTG->load("Button");
	regButton->setPosition(140, 135);
	regButton->setSize(200, 30);
	regButton->setText("http://speedblocks.se");
	regButton->connect("pressed", &LoginBox::regPressed, this);
	panel->add(regButton);

	tgui::Label::Ptr LiL3 = gui->themeTG->load("Label");
	LiL3->setText("Guest name");
	LiL3->setPosition(145, 180);
	panel->add(LiL3);

	tgui::EditBox::Ptr LiE3 = gui->themeTG->load("EditBox");
	LiE3->setPosition(100, 210);
	LiE3->setSize(200, 30);
	LiE3->connect("ReturnKeyPressed", &LoginBox::login, this, std::bind(&tgui::EditBox::getText, LiE3), "", 1);
	LiE3->connect("Focused Unfocused", &UI::chatFocus, gui, std::bind(&tgui::Widget::isFocused, LiE3));
	panel->add(LiE3);

	tgui::Button::Ptr LiB3 = gui->themeBB->load("Button");
	LiB3->setPosition(125, 250);
	LiB3->setSize(150, 40);
	LiB3->setOpacity(0.7);
	LiB3->setText("Login as Guest");
	LiB3->connect("pressed", &LoginBox::login, this, std::bind(&tgui::EditBox::getText, LiE3), "", 1);
	panel->add(LiB3);
}

void LoginBox::login(const sf::String& name, const sf::String& pass, sf::Uint8 guest) {
	if (guest && !name.getSize())
		return;
	gui->mainMenu->hide();
	hide();
	gui->connectingScreen->show();
	gui->window->draw(gui->resources.gfx.background); //Update the screen so a block on connect will show the connecting screen
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
	}
	else {
		gui->net.disconnect();
		gui->quickMsg("Could not connect to server");
		gui->connectingScreen->hide();
		show();
		gui->mainMenu->show();
	}
}

void LoginBox::closeLogin() {
	gui->mainMenu->enable();
	hide();
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
		system("start https://speedblocks.se");
	#elif __APPLE__
		system("open https://speedblocks.se");
	#else
		system("xdg-open https://speedblocks.se");
	#endif
}