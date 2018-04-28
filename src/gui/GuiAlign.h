#ifndef GUIALIGN_H
#define GUIALIGN_H

#include <TGUI/TGUI.hpp>
#include <type_traits>

template<typename... Ts> struct make_void { typedef void type;};
template<typename... Ts> using void_type = typename make_void<Ts...>::type;

#define MAKE_CHECK(FUNC, PARAM) \
template <typename T, typename = void> \
struct has_##FUNC : std::false_type {}; \
template <typename T> \
struct has_##FUNC<T, void_type<decltype( std::declval<T>()->FUNC(PARAM) )>> : std::true_type {};

#define CHECK(FUNC, PARAM) \
template <typename W, typename std::enable_if<has_##FUNC<W>{}, int>::type = 0> \
static void apply(W& w) { w-> FUNC(PARAM); } \
template <typename W, typename std::enable_if<!has_##FUNC<W>{}, int>::type = 0> \
static void apply(W& w) {}

struct Property {};

template <int WIDTH, int HEIGHT>
struct Size : Property {
  template <typename W>
  static void apply(W& w) {
    w->setSize(WIDTH, HEIGHT);
  }
};

template <int WIDTH>
struct Width : Property {
  template <typename W>
  static void apply(W& w) {
    w->setSize(WIDTH, w->getSize().y);
  }
};

template <int HEIGHT>
struct Height : Property {
  template <typename W>
  static void apply(W& w) {
    w->setSize(w->getSize().x, HEIGHT);
  }
};

template <int HEIGHT>
struct ScaleHeight : Property {
  template <typename W>
  static void apply(W& w) {
    float scale = HEIGHT / 100.f;
    w->setSize(w->getSize().x, w->getSize().y * scale);
  }
};

template <int WIDTH>
struct ScaleWidth : Property {
  template <typename W>
  static void apply(W& w) {
    float scale = WIDTH / 100.f;
    w->setSize(w->getSize().x * scale, w->getSize().y);
  }
};

struct Center : Property {
  template <typename W>
  static void apply(W& w) {
    w->setPosition(w->getPosition().x - w->getSize().x / 2, w->getPosition().y);
  }
};

struct Left : Property {
  template <typename W>
  static void apply(W& w) {
    w->setPosition(w->getPosition().x - w->getSize().x, w->getPosition().y);
  }
};

struct Hide : Property {
  template <typename W>
  static void apply(W& w) {
    w->hide();
  }
};

MAKE_CHECK(setHorizontalAlignment, tgui::Label::HorizontalAlignment::Center)

template <tgui::Label::HorizontalAlignment A>
struct TextAlign : Property {
  CHECK(setHorizontalAlignment, A)
};

MAKE_CHECK(setTextSize, 1)

template <int SIZE>
struct TextSize : Property {
  CHECK(setTextSize, SIZE)
};

struct Vertical {
  int size=0;
};

struct Horizontal {
  int size=0;
};

struct Gap {
  int size=0;
};

struct Move {
  int x=0, y=0;
};

struct Pos {
  Pos(int x, int y) : x(x), y(y) {}

  template <typename W>
  explicit Pos(W& w) : x(w->getPosition().x), y(w->getPosition().y), w(w->getSize().x), h(w->getSize().y) {}

  int x, y, w=0, h=0;
};

template <typename... T>
struct ApplyProperty;

template <typename First, typename... Rest>
struct ApplyProperty<First, Rest...> {
  template <typename W>
  static void apply(W& w) {
    First::apply(w);
    ApplyProperty<Rest...>::apply(w);
  }
};

template <>
struct ApplyProperty<> {
  template <typename W>
  static void apply(W&) {}
};

template <typename W>
using is_widget_t = typename std::enable_if<std::is_base_of<tgui::Widget, W>::value, int>::type;

template <typename P>
using is_property_t = typename std::enable_if<std::is_base_of<Property, P>::value, int>::type;

template <typename... T>
struct Align {
  Align(tgui::Panel::Ptr& base, int x, int y, int gap=0, bool down = true) : base(base.get()), x(x), y(y), gap(gap), step(0), down(down) {}

  Align(const Align& b) : base(b.base), x(b.x), y(b.y), gap(b.gap), step(b.step), down(b.down) {}

  template <typename W, is_widget_t<W> = 0>
  Align(W& w, int gap = 0, bool down = true) {
    base = static_cast<tgui::Panel*>(w->getParent());
    if (down) {
      x = w->getPosition().x;
      y = w->getPosition().y + w->getSize().y + gap;
    }
    else {
      x = w->getPosition().x + w->getSize().x + gap;
      y = w->getPosition().y;
    }
  }

  Align operator<<(Vertical d) { down=true; step=d.size; return *this; }
  Align operator<<(Horizontal r) { down=false; step=r.size; return *this; }
  Align operator<<(Gap g) { gap = g.size; return *this; }
  Align operator<<(Move s) {
    y += s.y;
    x += s.x;
    return *this;
  }
  Align operator<<(Pos p) {
    x = p.x; y = p.y;
    if (down && p.h)
      y += p.h + gap;
    else if (p.w)
      x += p.w + gap;
    return *this;
  }

  template <typename W, is_property_t<W> = 0>
  Align<T..., W> operator<<(const W& w) {
    return Align<T..., W>(*this);
  }

  template <typename W, is_widget_t<W> = 0>
  Align operator<<(std::shared_ptr<W>& w) {
    w->setPosition(x, y);
    apply(w);
    base->add(w);

    if (down) {
      if (step)
        y += step;
      else
        y += w->getSize().y + gap;
    }
    else {
      if (step)
        x += step;
      else
        x += w->getSize().x + gap;
    }

    return *this;
  }

private:
  template <typename W>
  void apply(W& widget) {
    ApplyProperty<T...>::apply(widget);
  }

  tgui::Panel* base;
  int x, y, gap, step;
  bool down;
};

#endif
