#ifndef BUTTONGRID_HPP
#define BUTTONGRID_HPP

#include <algorithm>
#include <vector>
#include "TguiWidgets.hpp"

namespace os {

class ButtonGrid : public PanelType<tgui::ScrollablePanel, ButtonGrid> {
   public:
    ButtonGrid& add(uint16_t id, LabeledButton button);

    LabeledButton& get_button(uint16_t id);
    ButtonGrid& del(uint16_t id);

    ButtonGrid& padding(float p);

    ButtonGrid& update();

   private:
    struct ButtonID {
        uint16_t id;
        LabeledButton button;
    };
    std::vector<ButtonID> button_ids;
    float padding_size = 0;
};

}  // namespace os

#endif  // BUTTONGRID_HPP