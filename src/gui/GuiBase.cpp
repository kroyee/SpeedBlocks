#include "GuiBase.h"
#include "Resources.h"

namespace tgui {
namespace SBGui {

GuiBase::GuiBase(sf::Rect<int> _pos, Resources& _res) : resources(_res) {
    panel = Panel::create();
    panel->setPosition(_pos.left, _pos.top);
    panel->setSize(_pos.width, _pos.height);
    panel->setBackgroundColor(sf::Color(255, 255, 255, 0));
    panel->hide();
    resources.gfx->tGui.add(panel);
}

GuiBase::GuiBase(sf::Rect<int> _pos, Resources& _res, tgui::Panel::Ptr parentPanel) : resources(_res) { panel = loadPanelTo(parentPanel, _pos); }

GuiBase::~GuiBase() {}

bool GuiBase::mouseOver(tgui::Widget::Ptr widget, int x, int y) {
    sf::Vector2f pos = widget->getAbsolutePosition();
    sf::Vector2f size = widget->getSize();
    if (x >= pos.x && x <= pos.x + size.x)
        if (y >= pos.y && y <= pos.y + size.y) return true;
    return false;
}

bool GuiBase::mouseOver(tgui::Widget::Ptr widget) {
    sf::Vector2f pos = widget->getAbsolutePosition();
    sf::Vector2f size = widget->getSize();
    sf::Vector2f mpos = resources.window.mapPixelToCoords(sf::Mouse::getPosition(resources.window));
    if (mpos.x >= pos.x - 2 && mpos.x <= pos.x + size.x + 2)
        if (mpos.y >= pos.y - 2 && mpos.y <= pos.y + size.y + 2) return true;
    return false;
}

}  // namespace SBGui
}  // namespace tgui
