#include "ScrollList.hpp"

#include <iostream>

namespace os {

ScrollList::ScrollList() {
#ifndef __APPLE__
    m_widget->setHorizontalScrollbarPolicy(tgui::Scrollbar::Policy::Never);
#endif
    m_widget->add(select_highlight.hide().get());
    select_highlight->setRenderer(defaultTheme.getRenderer("HighlightPanel"));
    m_widget->connect("MousePressed", [&](tgui::Vector2f pos) {
        int row_num = 0;
        while (pos.y > height) {
            ++row_num;
            pos.y -= height;
        }
        select_row(row_num);
    });
}

ScrollList& ScrollList::add(uint16_t id, std::vector<tgui::Widget::Ptr> w) {
    for (std::size_t i = 0, end = std::min(w.size(), positions.size()); i < end; ++i) {
        w[i]->setPosition(positions[i], 0);
        m_widget->add(w[i]);
    }
    for (auto& wid : w)
        if (wid->getSize().y + 10 > height) height = wid->getSize().y + 10;

    if (!std::any_of(rows.begin(), rows.end(), [&](Row& row) {
            if (row.id == id) {
                row.del();
                row = Row(id, std::move(w));
                return true;
            }
            return false;
        })) {
        rows.emplace_back(id, std::move(w));
    }

    update_positions();
    return *this;
}

void ScrollList::update_positions() {
    auto pos = height / 2;
    for (auto& row : rows) {
        row.pos(pos);
        pos += height;
    }
}

ScrollList& ScrollList::del(uint16_t id) {
    for (auto it = rows.begin(), end = rows.end(); it != end; ++it) {
        if (it->id == id) {
            it->del();
            rows.erase(it);
            break;
        }
    }

    update_positions();
    return *this;
}

void ScrollList::select_row(int row_num) {
    rows[row_num].selected = true;

    select_highlight.pos(0, row_num * height).size(m_widget->getSize().x - m_widget->getScrollbarWidth(), height).show();
    select_highlight.show();
}

void ScrollList::select_row() {
    select_highlight.hide();
    for (unsigned i = 0; i < rows.size(); ++i)
        if (rows[i].selected) select_row(i);
}

void ScrollList::remove_none_updated() {
    for (auto it = rows.begin(), end = rows.end(); it != end;) {
        if (it->updated)
            it = rows.erase(it);
        else
            ++it;
    }
}

ScrollList& ScrollList::pre_update() {
    for (auto& row : rows) row.updated = false;
    return *this;
}

ScrollList& ScrollList::post_update() {
    remove_none_updated();
    select_row();
    return *this;
}

}  // namespace os