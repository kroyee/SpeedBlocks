#include "AlertsUI.h"
#include <SFML/Network.hpp>
#include "GameSignals.h"
#include "NetworkPackets.hpp"
#include "Options.h"
#include "Resources.h"

static auto& PlaySound = Signal<void, int>::get("PlaySound");

AlertsUI::AlertsUI(sf::Rect<int> _pos, Resources& _res) : GuiBase(_pos, _res) {
    connectSignal("AddAlert", &AlertsUI::addAlert, this);

    PM::handle_packet([&](const NP_Alert& p) { addAlert(p.text); });
}

void AlertsUI::addAlert(const std::string& msg) {
    alertData newalert;
    newalert.id = idcount;
    idcount++;

    alerts.emplace_back();
    alertData& data = alerts.back();

    data.id = idcount++;
    data.label.text(msg).add_to(panel)->setMaximumTextWidth(520);
    data.button.connect("Pressed", &AlertsUI::removeAlert, this, data.id).add_to(panel);

    update();
    PlaySound(16);
}

void AlertsUI::removeAlert(uint16_t _id) {
    for (auto it = alerts.begin(); it != alerts.end(); it++)
        if (it->id == _id) {
            it->button.del();
            it->label.del();
            alerts.erase(it);
            update();
            return;
        }
}

void AlertsUI::removeAll() {
    for (auto& alert : alerts) {
        alert.button.del();
        alert.label.del();
    }
    alerts.clear();
    update();
}

void AlertsUI::update() {
    if (alerts.empty()) {
        hide();
        return;
    }
    show();
    unsigned posY = 0;
    for (auto& alert : alerts) {
        int height = alert.label->getSize().y;
        alert.button.pos(0, posY);
        alert.button.size(560, height + 20);
        alert.label.pos(20, posY + 10);
        posY += height + 30;
    }
    panel.height(posY - 10);
}
