#include "guiBase.h"

guiBase::guiBase(sf::Rect<int> _pos, Resources& _res) : resources(_res) {
    panel = tgui::Panel::create();
    panel->setPosition(_pos.left, _pos.top);
    panel->setSize(_pos.width, _pos.height);
    panel->setBackgroundColor(sf::Color(255, 255, 255, 0));
    panel->hide();
    resources.gfx->tGui.add(panel);
}

guiBase::guiBase(sf::Rect<int> _pos, Resources& _res, tgui::Panel::Ptr parentPanel) : resources(_res) { panel = loadPanelTo(parentPanel, _pos); }

guiBase::~guiBase() {}

bool guiBase::mouseOver(tgui::Widget::Ptr widget, int x, int y) {
    sf::Vector2f pos = widget->getAbsolutePosition();
    sf::Vector2f size = widget->getSize();
    if (x >= pos.x && x <= pos.x + size.x)
        if (y >= pos.y && y <= pos.y + size.y) return true;
    return false;
}

bool guiBase::mouseOver(tgui::Widget::Ptr widget) {
    sf::Vector2f pos = widget->getAbsolutePosition();
    sf::Vector2f size = widget->getSize();
    sf::Vector2f mpos = resources.window.mapPixelToCoords(sf::Mouse::getPosition(resources.window));
    if (mpos.x >= pos.x - 2 && mpos.x <= pos.x + size.x + 2)
        if (mpos.y >= pos.y - 2 && mpos.y <= pos.y + size.y + 2) return true;
    return false;
}

tgui::WidgetConverter guiBase::loadWidget(const std::string& widget, const PosAndSize& pos, const std::string& text) { return loadWidgetTo(panel, widget, pos, text); }

tgui::WidgetConverter guiBase::loadWidgetTo(tgui::Panel::Ptr _panel, const std::string& widget, const PosAndSize& pos, const std::string& text) {
    tgui::WidgetConverter wcnv = resources.gfx->themeTG->load(widget);
    tgui::Widget::Ptr wptr = wcnv;
    wptr->setPosition(pos.x, pos.y);
    if (pos.w || pos.h) wptr->setSize(pos.w, pos.h);

    if (text != "") {
        if (wptr->getWidgetType() == "Button")
            static_cast<tgui::Button*>(wptr.get())->setText(text);
        else if (wptr->getWidgetType() == "Label")
            static_cast<tgui::Label*>(wptr.get())->setText(text);
    }

    _panel->add(wptr);
    return wcnv;
}

tgui::Panel::Ptr guiBase::loadPanel(const PosAndSize& pos, bool hide) { return loadPanelTo(panel, pos, hide); }

tgui::Panel::Ptr guiBase::loadPanelTo(tgui::Panel::Ptr toPanel, const PosAndSize& pos, bool hide) {
    tgui::Panel::Ptr pan = tgui::Panel::create();
    pan->setPosition(pos.x, pos.y);
    pan->setSize(pos.w, pos.h);
    pan->setBackgroundColor(sf::Color(255, 255, 255, 0));

    toPanel->add(pan);
    if (hide) pan->hide();
    return pan;
}