#ifndef WIDGETALIGN_H
#define WIDGETALIGN_H

#include <type_traits>
#include "../helpers/MyTypeTraits.hpp"

namespace os {

struct Property {};

template <int WIDTH, int HEIGHT>
struct Size : Property {
    template <typename... W>
    static void apply(W&&... w) {
        (w.size(WIDTH, HEIGHT), ...);
    }
};

template <int WIDTH>
struct Width : Property {
    template <typename... W>
    static void apply(W&&... w) {
        (w.width(WIDTH), ...);
    }
};

template <int HEIGHT>
struct Height : Property {
    template <typename... W>
    static void apply(W&&... w) {
        (w.height(HEIGHT), ...);
    }
};

struct Center : Property {
    template <typename... W>
    static void apply(W&&... w) {
        (w.pos(w->getPosition().x - w->getSize().x / 2, w->getPosition().y), ...);
    }
};

struct Left : Property {
    template <typename... W>
    static void apply(W&&... w) {
        (w.pos(w->getPosition().x - w->getSize().x, w->getPosition().y), ...);
    }
};

struct Hide : Property {
    template <typename... W>
    static void apply(W&&... w) {
        (w.hide(), ...);
    }
};

template <typename T>
using has_set_text_size = decltype(std::declval<T>().text_size(1));

template <class T>
constexpr bool has_set_text_size_v = os::detect_v<has_set_text_size, T>;

template <int SIZE>
struct TextSize : Property {
    template <class W>
    static void apply_impl(W&& w) {
        if constexpr (has_set_text_size_v<W>) {
            w.text_size(SIZE);
        }
    }
    template <typename... W>
    static void apply(W&&... w) {
        (apply_impl(w), ...);
    }
};

struct Vertical {
    int size = 0;
};

struct Horizontal {
    int size = 0;
};

struct Gap {
    int size = 0;
};

struct Move {
    int x = 0, y = 0;
};

struct Pos {
    Pos(int x, int y) : x(x), y(y) {}

    template <typename W>
    explicit Pos(W& w) : x(w->getPosition().x), y(w->getPosition().y), w(w->getSize().x), h(w->getSize().y) {}

    int x, y, w = 0, h = 0;
};

template <typename... P>
struct ApplyProperty {
    template <typename W>
    static void apply(W& w) {
        (P::apply(w), ...);
    }

    template <typename... W>
    static void apply(W&... w) {
        (apply(w), ...);
    }
};

template <class BaseType>
struct AlignBase {
    template <typename... T>
    struct Align {
        Align(BaseType& base, int x, int y, int gap = 0, bool down = true) : base(base), x(x), y(y), gap(gap), step(0), down(down) {}

        Align(const Align& b) : base(b.base), x(b.x), y(b.y), gap(b.gap), step(b.step), down(b.down) {}

        /*template <typename W, is_widget_t<W> = 0>
        Align(W& w, int gap = 0, bool down = true) {
            base = static_cast<tgui::Panel*>(w->getParent());
            if (down) {
                x = w->getPosition().x;
                y = w->getPosition().y + w->getSize().y + gap;
            } else {
                x = w->getPosition().x + w->getSize().x + gap;
                y = w->getPosition().y;
            }
        }*/

        Align operator<<(Vertical d) {
            down = true;
            step = d.size;
            return *this;
        }
        Align operator<<(Horizontal r) {
            down = false;
            step = r.size;
            return *this;
        }
        Align operator<<(Gap g) {
            gap = g.size;
            return *this;
        }
        Align operator<<(Move s) {
            y += s.y;
            x += s.x;
            return *this;
        }
        Align operator<<(Pos p) {
            x = p.x;
            y = p.y;
            if (down && p.h)
                y += p.h + gap;
            else if (p.w)
                x += p.w + gap;
            return *this;
        }

        template <typename W>
        auto operator<<(W& w) {
            if constexpr (std::is_base_of_v<Property, W>) {
                return Align<T..., W>(*this);
            } else {
                w.pos(x, y);
                apply(w);
                base.add(w);

                if (down) {
                    if (step)
                        y += step;
                    else
                        y += w->getSize().y + gap;
                } else {
                    if (step)
                        x += step;
                    else
                        x += w->getSize().x + gap;
                }

                return *this;
            }
        }

       private:
        template <typename W>
        void apply(W& widget) {
            ApplyProperty<T...>::apply(widget);
        }

        BaseType& base;
        int x, y, gap, step;
        bool down;
    };
};

}  // namespace os

#endif
