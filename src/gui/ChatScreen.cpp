#include "ChatScreen.h"
#include <SFML/Network.hpp>
#include "GameSignals.h"
#include "Resources.h"

const unsigned int chatTextSize = 14;

ChatScreen::ChatScreen(sf::Rect<int> _pos, Resources& _res) : GuiBase(_pos, _res) {
    to = "Lobby";
    privto = "";
    hideLobbyChat = false;

    spamCount = 0;
    spamTime = sf::seconds(0);

    chatBox.pos(5, 5).size(470, 555).text_size(chatTextSize).hide().add_to(panel);

    chatBoxNoLobby.pos(5, 5).size(470, 555).text_size(chatTextSize).hide().add_to(panel);

    fadingChatBox.pos(5, 5).size(470, 555).text_size(chatTextSize).add_to(panel);

    hideLobby.text("Hide Lobby chat").pos(300, 40).hide().add_to(panel).connect({"Checked", "Unchecked"}, [&](bool b) {
        hideLobbyChat = b;
        chatBox->setVisible(!b);
        chatBoxNoLobby->setVisible(b);
    });

    input.pos(5, 565).size(470, 30).hide().add_to(panel).connect("ReturnKeyPressed", &ChatScreen::send, this);

    resources.gfx->tGui.add(sendToLabel.get());
    sendToLabel.pos(panel->getPosition().x - 100, panel->getPosition().y + 568).text("Lobby:").size(100, 25).hide().right();

    t1 = sf::Color(0, 0, 0, 200);
    t2 = sf::Color(0, 128, 0, 200);
    t3 = sf::Color(128, 0, 0, 200);
    t4 = sf::Color(50, 50, 50, 200);
    t5 = sf::Color(128, 50, 50, 200);

    Net::takePacket(12, [&](sf::Packet& packet) {
        std::string name, msg;
        uint8_t type;
        packet >> type >> name >> msg;
        if (type == 3) privto = name;
        addLine(name + ": " + msg, type);
    });
}

void ChatScreen::activate() {
    if (hideLobbyChat)
        chatBoxNoLobby.show();
    else
        chatBox.show();
    sendToLabel.show();
    input.show();
    hideLobby.show();
    input.focus();
    fadingChatBox.hide();
}

void ChatScreen::deactivate() {
    sendToLabel.hide();
    input.hide();
    hideLobby.hide();
    fadingChatBox.show();
    chatBoxNoLobby.hide();
    chatBox.hide();
}

void ChatScreen::send() {
    deactivate();
    std::string msg = input->getText();
    input->setText("");
    while (!msg.empty()) {
        if (msg[0] == ' ')
            msg.erase(0);
        else
            break;
    }
    if (!msg.size()) return;

    if (spamCount > 7000) {
        addLine("I HAVE TO STOP SPAMMING THE CHAT!!!", 4);
        spamCount = 12000;
        return;
    }
    if (msg.size() > 3)
        if (msg[0] == '/' && msg[1] == 'w' && msg[2] == ' ') {
            short until = msg.find(' ', 3);
            std::string _privto = msg.substr(3, until - 3);
            std::string privmsg = msg.substr(until, std::string::npos);
            sendMsg(_privto, privmsg);
            addLine(resources.name + ": " + privmsg, 5);
            spamCount += 2000;
            sendTo(_privto, true);
            return;
        }
    std::string postmsg = resources.name + ": " + msg;

    for (unsigned int i = 0; i < msg.size(); i++)
        if (msg[i] != ' ') {
            if (to == "Lobby" || to == "Room")
                addLine(postmsg, 4);
            else
                addLine(postmsg, 5);
            sendMsg(to, msg);
            spamCount += 2000;
            return;
        }
}

static auto& SendPacket = Signal<void, sf::Packet&>::get("SendPacket");

void ChatScreen::sendMsg(const std::string& to, const std::string& msg) {
    sf::Packet packet;
    packet << (uint8_t)10;
    if (to == "Room")
        packet << (uint8_t)1 << msg;
    else if (to == "Lobby")
        packet << (uint8_t)2 << msg;
    else
        packet << (uint8_t)3 << to << msg;
    SendPacket(packet);
}

void ChatScreen::addLine(const std::string& msg, uint8_t type) {  // 1=room, 2=lobby, 3=priv, 4=self, 5=privself
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
        fadeTime.push_front(resources.delayClock.getElapsedTime() + sf::seconds(5));
    }
    if (type != 2) chatBoxNoLobby->addLine(msg, color);
}

void ChatScreen::fade(const sf::Time& t) {
    while (!fadeTime.empty()) {
        if (t > fadeTime.back()) {
            fadeTime.pop_back();
            fadingChatBox->removeLine(0);
        } else
            break;
    }

    uint8_t amount = fadingChatBox->getLineAmount() * fadingChatBox->getTextSize();
    if (!amount || isActive())
        fadingChatBox.hide();
    else {
        fadingChatBox.show().pos(5, 560 - amount).size(470, amount);
    }

    sf::Time tmp = t - spamTime;
    spamCount -= tmp.asMilliseconds();
    spamTime = t;
    if (spamCount < 0) spamCount = 0;
}

bool ChatScreen::handleEvent(sf::Event& event) {
    if (event.type == sf::Event::KeyPressed)
        if (panel->isVisible()) {
            if (!chatBox->isVisible()) {
                if (event.key.code == sf::Keyboard::Return) {
                    activate();
                    return true;
                }
            } else
                input.focus();
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
    if (input->isVisible()) input.focus();
}

void ChatScreen::sendTo(const std::string& _to, bool force) {
    if (to != "Lobby" && to != "Room" && !force) return;
    to = _to;
    sendToLabel->setText(to + ':');
}

void ChatScreen::toggleSendTo() {
    if (to == "Lobby") {
        if (resources.gamestate != GameStates::MainMenu && resources.gamestate != GameStates::Replay)
            sendTo("Room");
        else if (privto != "")
            sendTo(privto);
    } else if (to == "Room") {
        if (privto != "")
            sendTo(privto);
        else
            sendTo("Lobby");
    } else
        sendTo("Lobby", true);
}

void ChatScreen::dark() {
    t1 = sf::Color(255, 255, 255, 200);
    t2 = sf::Color(255, 128, 255, 200);
    t3 = sf::Color(128, 255, 255, 200);
    t4 = sf::Color(205, 205, 205, 200);
    t5 = sf::Color(128, 205, 205, 200);

    tgui::ChatBoxRenderer(os::defaultTheme.getRenderer("ChatBox")).setBackgroundColor(sf::Color(55, 55, 55, 128));
}

void ChatScreen::light() {
    t1 = sf::Color(0, 0, 0, 200);
    t2 = sf::Color(0, 128, 0, 200);
    t3 = sf::Color(128, 0, 0, 200);
    t4 = sf::Color(50, 50, 50, 200);
    t5 = sf::Color(128, 50, 50, 200);

    tgui::ChatBoxRenderer(os::defaultTheme.getRenderer("ChatBox")).setBackgroundColor(sf::Color(200, 200, 200, 128));
}
