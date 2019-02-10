#ifndef TGUI_WIDGETS
#define TGUI_WIDGETS

#include <TGUI/TGUI.hpp>
#include "../helpers/MyTypeTraits.hpp"
#include "WidgetAlign.h"

namespace os {

inline tgui::Theme defaultTheme;

using tgui::bindBottom;
using tgui::bindHeight;
using tgui::bindLeft;
using tgui::bindPosition;
using tgui::bindRight;
using tgui::bindSize;
using tgui::bindTop;
using tgui::bindWidth;

template <class PanelT>
struct Panel_impl;

template <class T>
using has_set_opacity = decltype(std::declval<T::m_widget>()->setOpacity(0.5));

template <class T>
constexpr bool has_set_opacity_v = os::detect_v<has_set_opacity, T>;

template <typename WidgetType, typename ReturnType_>
class Widget {
   public:
    using ReturnType = ReturnType_;
    Widget() : m_widget(WidgetType::create()) {}

    template <class T>
    Widget(T&& t) : m_widget(WidgetType::create(std::forward<T>(t))) {}

    ReturnType& pos(float x, float y) {
        m_widget->setPosition(x, y);
        return *this;
    }

    ReturnType& pos(const sf::Vector2f& position) { return pos(position.x, position.y); }

    ReturnType& size(float x, float y) {
        m_widget->setSize(x, y);
        return *this;
    }

    ReturnType& size(const sf::Vector2f& vec) { return size(vec.x, vec.y); }

    ReturnType& width(float w) { return size(w, m_widget->getSize().y); }

    ReturnType& height(float h) { return size(m_widget->getSize().x, h); }

    ReturnType& show(bool val = true) {
        m_widget->setVisible(val);
        return *this;
    }

    ReturnType& hide() {
        m_widget->setVisible(false);
        return *this;
    }

    ReturnType& enable(bool val = true) {
        m_widget->setEnabled(val);
        return *this;
    }

    ReturnType& disable() {
        m_widget->setEnabled(false);
        return *this;
    }

    ReturnType& focus(bool val = true) {
        m_widget->setFocused(val);
        return *this;
    }

    template <typename T>
    ReturnType& add_to(T& panel) {
        panel->add(m_widget);
        return *this;
    }

    ReturnType& del() {
        m_widget->getParent()->remove(m_widget);
        return *this;
    }

    ReturnType& opacity(float val) {
        if constexpr (has_set_opacity_v<WidgetType>) m_widget->setOpacity(val);
        return *this;
    }

    template <typename... T>
    ReturnType& connect(T&&... t) {
        m_widget->connect(std::forward<T>(t)...);
        return *this;
    }

    template <typename... T>
    ReturnType& connect(std::initializer_list<std::string> l, T&&... t) {
        m_widget->connect(std::move(l), std::forward<T>(t)...);
        return *this;
    }

    typename WidgetType::Ptr& operator->() { return m_widget; }

    typename WidgetType::Ptr& get() { return m_widget; }

    operator ReturnType&() { return static_cast<ReturnType&>(*this); }

   protected:
    typename WidgetType::Ptr m_widget;
};  // namespace os

template <typename WidgetType, typename ReturnType_>
class TextWidget : public Widget<WidgetType, ReturnType_> {
   public:
    using WType = Widget<WidgetType, ReturnType_>;
    using ReturnType = ReturnType_;
    using Widget<WidgetType, ReturnType>::operator ReturnType&;
    ReturnType& text(std::string text) {
        this->m_widget->setText(std::move(text));
        return *this;
    }

    ReturnType& text_size(unsigned size) {
        this->m_widget->setTextSize(size);
        return *this;
    }

    ReturnType& center() {
        if constexpr (std::is_same_v<WidgetType, tgui::Label>)
            this->m_widget->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Center);
        else if constexpr (std::is_same_v<WidgetType, tgui::EditBox>)
            this->m_widget->setAlignment(tgui::EditBox::Alignment::Center);

        return *this;
    }

    ReturnType& right() {
        if constexpr (std::is_same_v<WidgetType, tgui::Label>)
            this->m_widget->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Right);
        else if constexpr (std::is_same_v<WidgetType, tgui::EditBox>)
            this->m_widget->setAlignment(tgui::EditBox::Alignment::Right);

        return *this;
    }
};

class Picture : public Widget<tgui::Picture, Picture> {
   public:
    template <typename T>
    Picture(T&& t) : Widget<tgui::Picture, Picture>(std::forward<T>(t)) {}
    Picture() : Widget<tgui::Picture, Picture>(default_texture) {}

    inline static tgui::Texture default_texture;
};

template <typename PanelT, class ReturnType_>
class PanelType : public Widget<PanelT, ReturnType_> {
   public:
    using ReturnType = ReturnType_;
    using Widget<PanelT, ReturnType>::operator ReturnType&;
    template <typename... WType>
    ReturnType& add(WType&&... widget) {
        (std::forward<WType>(widget).add_to(*this), ...);
        return *this;
    }

    ReturnType& background() {
        Picture pic;
        pic->setSize(tgui::bindSize(this->m_widget));
        this->m_widget->add(pic.get());
        return *this;
    }

    ReturnType& background(const sf::Color& color) {
        this->m_widget->getRenderer()->setBackgroundColor(color);
        return *this;
    }

    template <class... T>
    using Align = typename AlignBase<PanelType>::template Align<T...>;

    Align<> align(int x, int y, int gap = 0, bool down = true) { return Align<>(*this, x, y, gap, down); }

    template <typename... T>
    Align<T...> align(int x, int y, int gap = 0, bool down = true) {
        return Align<T...>(*this, x, y, gap, down);
    }

    template <typename W>
    Align<> under(W& w) {
        return {w};
    }

    template <typename W>
    Align<> right_of(W& w) {
        return (Align<>{w, 0, false} << Vertical{0});
    }
};

template <class PanelT>
struct Panel_impl : public PanelType<PanelT, Panel_impl<PanelT>> {};

using Panel = Panel_impl<tgui::Panel>;
using ScrollPanel = Panel_impl<tgui::ScrollablePanel>;

class Label : public TextWidget<tgui::Label, Label> {
   public:
    Label& small();
    Label& medium();
    Label& large();

    Label& max(int val);
};

template <typename WidgetType>
class TitledWidget : public WidgetType {
   public:
    using ReturnType = typename WidgetType::ReturnType;
    using WidgetType::operator ReturnType&;
    TitledWidget() { update_title(); }

    ReturnType& title(const std::string& text) {
        m_title.text(text);
        return *this;
    }

    ReturnType& title_size(unsigned size) {
        m_title.text_size(size);
        return *this;
    }

    ReturnType& title_top() {
        m_title_position = TitlePos::Top;
        update_title();
        return *this;
    }
    ReturnType& title_left() {
        m_title_position = TitlePos::Left;
        update_title();
        return *this;
    }
    ReturnType& title_on_top() {
        m_title_position = TitlePos::OnTop;
        update_title();
        return *this;
    }

    ReturnType& add_to(Panel_impl<tgui::Panel>& panel) {
        panel->add(m_title.get());
        panel->add(this->m_widget);
        return *this;
    }

    ReturnType& del() {
        auto parent = this->m_widget->getParent();
        parent->remove(this->m_widget);
        parent->remove(m_title.get());
        return *this;
    }

   protected:
    Label m_title;

   private:
    enum class TitlePos { Top, Left, OnTop };

    TitlePos m_title_position = TitlePos::Left;

    void update_title() {
        if (m_title_position == TitlePos::Top) {
            m_title->setPosition(bindLeft(this->m_widget) + (bindWidth(this->m_widget) - bindWidth(m_title.get())) / 2,
                                 bindTop(this->m_widget) - bindHeight(m_title.get()) - 5);
        } else if (m_title_position == TitlePos::Left) {
            m_title->setPosition(bindLeft(this->m_widget) - bindWidth(m_title.get()) - 10, bindTop(this->m_widget) + 5);
        } else {
            m_title->setPosition(bindLeft(this->m_widget) + (bindWidth(this->m_widget) - bindWidth(m_title.get())) / 2.0,
                                 bindTop(this->m_widget) + (bindHeight(this->m_widget) - bindHeight(m_title.get())) / 2.0);
        }
    }
};

class Button : public TextWidget<tgui::Button, Button> {
   public:
    Button& small();
    Button& medium();
    Button& large();
};

class TitledButton : public TitledWidget<TextWidget<tgui::Button, TitledButton>> {};

class LabeledButton : public Widget<tgui::Panel, LabeledButton> {
   public:
    LabeledButton();

    LabeledButton& text(const std::string& text);
    LabeledButton& text_size(int size);
    LabeledButton& label_size(int size);
    LabeledButton& labels(const std::string& tl, const std::string& tr, const std::string& bl, const std::string& br);
    LabeledButton& padding(float p);

    Label top_left, top_right, bottom_left, bottom_right;

   private:
    Button button;
};

class Edit : public TitledWidget<TextWidget<tgui::EditBox, Edit>> {
   public:
    Edit(const std::string& title = "");

    Edit& small();
    Edit& medium();
    Edit& large();

    Edit& num();
    Edit& pass();
};

template <class WidgetType>
class CheckableWidget : public WidgetType {
   public:
    using ReturnType = typename WidgetType::ReturnType;
    using WidgetType::operator ReturnType&;

    ReturnType& check(bool val) {
        this->m_widget->setChecked(val);
        return *this;
    }

    ReturnType& check() {
        this->m_widget->setChecked(true);
        return *this;
    }

    ReturnType& uncheck() {
        this->m_widget->setChecked(false);
        return *this;
    }
};

class CheckBox : public CheckableWidget<TextWidget<tgui::CheckBox, CheckBox>> {
   public:
    CheckBox& small();
    CheckBox& medium();
    CheckBox& large();
};

class RadioButton : public CheckableWidget<TextWidget<tgui::RadioButton, RadioButton>> {
   public:
    RadioButton& small();
    RadioButton& medium();
    RadioButton& large();
};

class ListBox : public Widget<tgui::ListBox, ListBox> {
   public:
    ListBox& add(std::string str);
    ListBox& remove(std::string str);
    ListBox& clear();
};

class TextBox : public Widget<tgui::TextBox, TextBox> {
   public:
    TextBox& text(std::string& text) {
        m_widget->setText(text);
        return *this;
    }
};

class ChatBox : public Widget<tgui::ChatBox, ChatBox> {
   public:
    ChatBox& text_size(int size);
};

class Slider : public TitledWidget<Widget<tgui::Slider, Slider>> {
   public:
    Slider& min(float value);
    Slider& max(float value);
    Slider& step(float value);
    Slider& set(float val);
};

class ProgressBar : public TitledWidget<Widget<tgui::ProgressBar, ProgressBar>> {
   public:
    ProgressBar& min(unsigned val);
    ProgressBar& max(unsigned val);
    ProgressBar& set(unsigned val);
};

class Tabs : public Widget<tgui::Tabs, Tabs> {
   public:
    Tabs& add(std::string str);
    Tabs& select(int val);
    Tabs& tab_height(float val);
};

}  // namespace os

#endif
