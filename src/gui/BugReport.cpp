#include "BugReport.h"
#include "Resources.h"
#include "network.h"

BugReport::BugReport(sf::Rect<int> _pos, Resources& _res, tgui::Panel::Ptr parent) : GuiBase(_pos, _res, parent) {
    join = false;

    panel.add(os::Label().text("What happened?").pos(10, 5), os::Label().text("What dod you expect to happen?").pos(10, 125),
              os::Label().text("How can we reproduce this?").pos(10, 245), os::Label().text("How can we contact you with questions?").pos(10, 365));

    os::Size<550, 80>::(happened.pos(5, 40), expected.pos(5, 160), reproduce.pos(5, 220), contact.pos(5, 400));

    os::Button().text("Send").pos(250, 495).size(60, 30).connect("Pressed", &BugReport::sendReport, this).add_to(panel);
}

void BugReport::sendReport() {
    std::string shappened = happened->getText();
    std::string sexpected = expected->getText();
    std::string sreproduce = reproduce->getText();
    std::string scontact = contact->getText();

#ifdef _WIN32
    std::string os = "Win";
#elif __APPLE__
    std::string os = "Mac";
#else
    std::string os = "Linux";
#endif

    std::string version = std::to_string(resources.clientVersion);

    std::string postfield = "{\"happening\":\"" + shappened + "\",\"supposed\":\"" + sexpected + "\",\"reproduce\":\"" + sreproduce + "\",\"contact\":\"" + scontact +
                            "\",\"version\":\"" + version + "\",\"os\":\"" + os + "\"}";

    t = std::thread([&]() {
        resources.net->sendCurlPost("https://bugs.speedblocks.se/bugs", postfield, 0);
        join = true;
    });

    happened->setText("");
    expected->setText("");
    reproduce->setText("");
    contact->setText("");
}
