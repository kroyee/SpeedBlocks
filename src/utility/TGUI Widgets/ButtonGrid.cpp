#include "ButtonGrid.hpp"

namespace os {

ButtonGrid& ButtonGrid::add(uint16_t id, LabeledButton button) {
    if (!std::any_of(button_ids.begin(), button_ids.end(), [&](ButtonID& b) {
            if (b.id == id) {
                m_widget->remove(b.button.get());
                b.button = button;
                m_widget->add(b.button.get());
                return true;
            }
            return false;
        })) {
        m_widget->add(button.get());
        button_ids.push_back({id, button});
    }
    if (padding_size) {
        button.padding(padding_size);
    }
    return *this;
}

LabeledButton& ButtonGrid::get_button(uint16_t id) {
    static LabeledButton def;

    for (auto& b : button_ids)
        if (b.id == id) return b.button;

    return def;
}

ButtonGrid& ButtonGrid::del(uint16_t id) {
    for (auto it = button_ids.begin(), end = button_ids.end(); it != end; ++it) {
        if (it->id == id) {
            m_widget->remove(it->button.get());
            button_ids.erase(it);
            return *this;
        }
    }

    return *this;
}

ButtonGrid& ButtonGrid::padding(float p) {
    padding_size = p;
    for (auto& b : button_ids) b.button.padding(p);

    return *this;
}

ButtonGrid& ButtonGrid::update() {
    if (!button_ids.empty()) {
        float x = 0, y = 0;
        float parentWidth = m_widget->getSize().x - m_widget->getScrollbarWidth();

        for (auto& b : button_ids) {
            auto [width, height] = b.button->getSize();
            if (x + width > parentWidth) {
                x = 0;
                y += height;
            }
            b.button.pos(x, y);
            x += width;
        }
    }
    return *this;
}

}  // namespace os