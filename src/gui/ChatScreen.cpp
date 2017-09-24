#include "ChatScreen.h"
#include "Signal.h"
#include <SFML/Network.hpp>

const unsigned int chatTextSize = 14;

ChatScreen::ChatScreen(sf::Rect<int> _pos, Resources& _res) : guiBase(_pos, _res) {
	panel->disable(false);
	to = "Lobby";
	privto = "";
	hideLobbyChat=false;

	panel->disable(false);
	spamCount=0;
	spamTime=sf::seconds(0);

	chatBox = resources.gfx->themeTG->load("ChatBox");
	chatBox->setPosition(5, 5);
	chatBox->setSize(470, 555);
	chatBox->getRenderer()->setBackgroundColor(sf::Color(200,200,200,128));
	chatBox->getRenderer()->setBorders({0,0,0,0});
	chatBox->setTextSize(chatTextSize);
	chatBox->disable(false);
	chatBox->hide();
	panel->add(chatBox);

	chatBoxNoLobby = resources.gfx->themeTG->load("ChatBox");
	chatBoxNoLobby->setPosition(5, 5);
	chatBoxNoLobby->setSize(470, 555);
	chatBoxNoLobby->getRenderer()->setBackgroundColor(sf::Color(200,200,200,128));
	chatBoxNoLobby->getRenderer()->setBorders({0,0,0,0});
	chatBoxNoLobby->setTextSize(chatTextSize);
	chatBoxNoLobby->disable(false);
	chatBoxNoLobby->hide();
	panel->add(chatBoxNoLobby);

	fadingChatBox = resources.gfx->themeTG->load("ChatBox");
	fadingChatBox->setPosition(5, 5);
	fadingChatBox->setSize(470, 555);
	fadingChatBox->getRenderer()->setBackgroundColor(sf::Color(200,200,200,128));
	fadingChatBox->getRenderer()->setBorders({0,0,0,0});
	fadingChatBox->setTextSize(chatTextSize);
	fadingChatBox->disable(false);
	panel->add(fadingChatBox);

	hideLobby = resources.gfx->themeTG->load("CheckBox");
	hideLobby->setText("Hide Lobby chat");
	hideLobby->setPosition(panel->getPosition().x+300, panel->getPosition().y+40);
	hideLobby->connect("Checked", [&](){ hideLobbyChat=true; chatBox->hide(); chatBoxNoLobby->show(); });
	hideLobby->connect("Unchecked", [&](){ hideLobbyChat=false; chatBox->show(); chatBoxNoLobby->hide(); });
	hideLobby->hide();
	resources.gfx->tGui.add(hideLobby);

	input = resources.gfx->themeTG->load("EditBox");
	input->setPosition(panel->getPosition().x+5, panel->getPosition().y+565);
	input->setSize(470, 30);
	input->connect("ReturnKeyPressed", &ChatScreen::send, this);
	input->hide();
	resources.gfx->tGui.add(input);

	sendToLabel = resources.gfx->themeTG->load("Label");
	sendToLabel->setPosition(panel->getPosition().x-100, panel->getPosition().y+568);
	sendToLabel->setText("Lobby:");
	sendToLabel->setSize(100, 25);
	sendToLabel->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Right);
	sendToLabel->disable(false);
	sendToLabel->hide();
	resources.gfx->tGui.add(sendToLabel);

	t1 = sf::Color(0,0,0,200);
	t2 = sf::Color(0,128,0,200);
	t3 = sf::Color(128,0,0,200);
	t4 = sf::Color(50,50,50,200);
	t5 = sf::Color(128,50,50,200);

	Net::takePacket(12, [&](sf::Packet &packet){
		sf::String name, msg;
		sf::Uint8 type;
		packet >> type >> name >> msg;
		if (type == 3)
			privto = name;
		addLine(name + ": " + msg, type);
	});
}

void ChatScreen::activate() {
	if (hideLobbyChat)
		chatBoxNoLobby->show();
	else
		chatBox->show();
	sendToLabel->show();
	input->show();
	hideLobby->show();
	input->focus();
	fadingChatBox->hide();
}

void ChatScreen::deactivate() {
	sendToLabel->hide();
	input->hide();
	hideLobby->hide();
	fadingChatBox->show();
	chatBoxNoLobby->hide();
	chatBox->hide();
}

void ChatScreen::send() {
	deactivate();
	sf::String msg = input->getText();
	input->setText("");
	while (!msg.isEmpty()) {
		if (msg[0] == ' ')
			msg.erase(0);
		else
			break;
	}
	if (!msg.getSize())
		return;
	
	if (spamCount>7000) {
		addLine("I HAVE TO STOP SPAMMING THE CHAT!!!", 4);
		spamCount=12000;
		return;
	}
	if (msg.getSize() > 3)
		if (msg[0]=='/' && msg[1]=='w' && msg[2]==' ') {
			short until = msg.find(' ', 3);
			sf::String _privto = msg.substring(3, until-3);
			sf::String privmsg = msg.substring(until, sf::String::InvalidPos);
			sendMsg(_privto, privmsg);
			addLine(resources.name + ": " + privmsg, 5);
			spamCount+=2000;
			sendTo(_privto, true);
			return;
		}
	sf::String postmsg = resources.name + ": " + msg;

	for (unsigned int i=0; i<msg.getSize(); i++)
		if (msg[i] != ' ') {
			if (to == "Lobby" || to == "Room")
				addLine(postmsg, 4);
			else
				addLine(postmsg, 5);
			sendMsg(to, msg);
			spamCount+=2000;
			return;
		}
}

void ChatScreen::sendMsg(const sf::String& to, const sf::String& msg) {
	sf::Packet packet;
	packet << (sf::Uint8)10;
	if (to == "Room")
		packet << (sf::Uint8)1 << msg;
	else if (to == "Lobby")
		packet << (sf::Uint8)2 << msg;
	else
		packet << (sf::Uint8)3 << to << msg;
	Signals::SendPacket(packet);
}

void ChatScreen::addLine(const sf::String& msg, sf::Uint8 type) { //1=room, 2=lobby, 3=priv, 4=self, 5=privself
	sf::Color color = t1;
	if (type == 2)
		color = t2;
	else if (type == 3)
		color = t3;
	else if (type == 4)
		color = t4;
	else if (type == 5)
		color = t5;

	chatBox->addLine(msg, color);
	if (type != 2 || !hideLobbyChat) {
		fadingChatBox->addLine(msg, color);
		fadeTime.push_front(resources.delayClock.getElapsedTime()+sf::seconds(5));
	}
	if (type != 2)
		chatBoxNoLobby->addLine(msg, color);
}

void ChatScreen::fade(const sf::Time& t) {
	while (!fadeTime.empty()) {
		if (t > fadeTime.back()) {
			fadeTime.pop_back();
			fadingChatBox->removeLine(0);
		}
		else break;
	}

	sf::Uint8 amount = fadingChatBox->getScrollbar()->getMaximum();
	if (!amount || isActive())
		fadingChatBox->hide();
	else {
		fadingChatBox->show();
		fadingChatBox->setPosition(5, 560-amount);
		fadingChatBox->setSize(470, amount);
	}

	sf::Time tmp = t - spamTime;
	spamCount-=tmp.asMilliseconds();
	spamTime = t;
	if (spamCount<0)
		spamCount=0;
}

bool ChatScreen::handleEvent(sf::Event& event) {
	if (event.type == sf::Event::KeyPressed)
		if (panel->isVisible()) {
			if (!chatBox->isVisible()) {
				if (event.key.code == sf::Keyboard::Return) {
					activate();
					return true;
				}
			}
			else input->focus();
		}
	return false;
}

bool ChatScreen::isActive() {
	if (chatBox->isVisible() || chatBoxNoLobby->isVisible())
		return true;
	else
		return false;
}

void ChatScreen::focus() {
	if (input->isVisible())
		input->focus();
}

void ChatScreen::sendTo(const sf::String& _to, bool force) {
	if (to != "Lobby" && to != "Room" && !force)
		return;
	to = _to;
	sendToLabel->setText(to + ':');
}

void ChatScreen::toggleSendTo() {
	if (to == "Lobby") {
		if (resources.gamestate != GameStates::MainMenu && resources.gamestate != GameStates::Replay)
			sendTo("Room");
		else if (privto != "")
			sendTo(privto);
	}
	else if (to == "Room") {
		if (privto != "")
			sendTo(privto);
		else
			sendTo("Lobby");
	}
	else
		sendTo("Lobby", true);
}

void ChatScreen::dark() {
	t1 = sf::Color(255,255,255,200);
	t2 = sf::Color(255,128,255,200);
	t3 = sf::Color(128,255,255,200);
	t4 = sf::Color(205,205,205,200);
	t5 = sf::Color(128,205,205,200);

	chatBox->getRenderer()->setBackgroundColor(sf::Color(55,55,55,128));
	chatBoxNoLobby->getRenderer()->setBackgroundColor(sf::Color(55,55,55,128));
	fadingChatBox->getRenderer()->setBackgroundColor(sf::Color(55,55,55,128));
}

void ChatScreen::light() {
	t1 = sf::Color(0,0,0,200);
	t2 = sf::Color(0,128,0,200);
	t3 = sf::Color(128,0,0,200);
	t4 = sf::Color(50,50,50,200);
	t5 = sf::Color(128,50,50,200);

	chatBox->getRenderer()->setBackgroundColor(sf::Color(200,200,200,128));
	chatBoxNoLobby->getRenderer()->setBackgroundColor(sf::Color(200,200,200,128));
	fadingChatBox->getRenderer()->setBackgroundColor(sf::Color(200,200,200,128));
}