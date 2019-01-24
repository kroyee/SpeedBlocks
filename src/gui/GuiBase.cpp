#include "GuiBase.h"
#include "Resources.h"

namespace SBGui {

GuiBase::GuiBase(sf::Rect<int> _pos, Resources& _res) : resources(_res) {
    panel.pos(_pos.left, _pos.top).size(_pos.width, _pos.height).hide();
    resources.gfx->tGui.add(panel.get());
}

GuiBase::GuiBase(sf::Rect<int> _pos, Resources& _res, os::Panel& parentPanel) : resources(_res) {
    panel.pos(_pos.left, _pos.top).size(_pos.width, _pos.height).add_to(parentPanel);
}

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
