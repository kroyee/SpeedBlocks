#ifndef BUGREPORT_H
#define BUGREPORT_H

#include <thread>
#include "GuiBase.h"

class Resources;

class BugReport : public GuiBase {
   public:
    sf::Rect<int> pos;

    os::TextBox happened;
    os::TextBox expected;
    os::TextBox reproduce;
    os::TextBox contact;

    bool join;

    std::thread t;

    BugReport(sf::Rect<int> _pos, Resources& _res, os::Panel& parent);

    void sendReport();
};

#endif