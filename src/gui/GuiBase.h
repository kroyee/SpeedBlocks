#ifndef GUIBASE_H
#define GUIBASE_H

#include <TGUI/TGUI.hpp>
#include <type_traits>
#include "Textures.h"
#include "TguiWidgets.hpp"
#include "WidgetAlign.h"

class Resources;

namespace SBGui {

struct PosAndSize {
    PosAndSize() : x(0), y(0), w(960), h(600) {}
    PosAndSize(float _x, float _y, float _w, float _h) : x(_x), y(_y), w(_w), h(_h) {}
    PosAndSize(float _x, float _y) : x(_x), y(_y), w(0), h(0) {}
    PosAndSize(sf::Rect<int> rect) : x(rect.left), y(rect.top), w(rect.width), h(rect.height) {}
    PosAndSize(sf::Rect<float> rect) : x(rect.left), y(rect.top), w(rect.width), h(rect.height) {}
    float x, y, w, h;
};

class GuiBase {
   public:
    os::Panel panel;
    Resources& resources;

    GuiBase(sf::Rect<int> _pos, Resources& _res);
    GuiBase(sf::Rect<int> _pos, Resources& _res, os::Panel& parentPanel);
    virtual ~GuiBase();
    virtual void show() { panel.show(); }
    virtual void hide() { panel.hide(); }
    void enable() { panel.enable(); }
    void disable() { panel.disable(); }
    bool isVisible() { return panel->isVisible(); }

    bool mouseOver(tgui::Widget::Ptr widget, int x, int y);
    bool mouseOver(tgui::Widget::Ptr widget);

    template <class... T>
    using Align = typename os::AlignBase<decltype(panel)>::template Align<T...>;

    Align<> align(int x, int y, int gap = 0, bool down = true) { return Align<>(panel, x, y, gap, down); }

    template <typename... T>
    Align<T...> align(int x, int y, int gap = 0, bool down = true) {
        return Align<T...>(panel, x, y, gap, down);
    }

    template <typename W>
    Align<> under(W& w) {
        return {w};
    }

    template <typename W>
    Align<> right_of(W& w) {
        return (Align<>{w, 0, false} << os::Vertical{0});
    }
};

}  // namespace SBGui

using GuiBase = SBGui::GuiBase;

#endif
