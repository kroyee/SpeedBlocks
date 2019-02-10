#include "TguiWidgets.hpp"

namespace os {

Button& Button::small() { return size(100, 30).text_size(18); }

Button& Button::medium() { return size(150, 40).text_size(24); }

Button& Button::large() { return size(200, 50).text_size(32); }

LabeledButton::LabeledButton() {
    button->setRenderer(defaultTheme.getRenderer("BigButton"));
    m_widget->add(top_left.get());
    m_widget->add(top_right.get());
    m_widget->add(bottom_left.get());
    m_widget->add(bottom_right.get());
    m_widget->add(button.get());

    top_left->setPosition(bindLeft(button.get()) + 5, bindTop(button.get()) + 5);
    top_right->setPosition(bindRight(button.get()) - bindWidth(top_right.get()) - 5, bindTop(button.get()) + 5);
    bottom_left->setPosition(bindLeft(button.get()) + 5, bindBottom(button.get()) - bindHeight(bottom_left.get()) - 5);
    bottom_right->setPosition(bindRight(button.get()) - bindWidth(bottom_right.get()) - 5, bindBottom(button.get()) - bindHeight(bottom_right.get()) - 5);

    button->setSize(bindSize(m_widget));
}

LabeledButton& LabeledButton::text(const std::string& text) {
    button.text(text);
    return *this;
}

LabeledButton& LabeledButton::text_size(int size) {
    button.text_size(size);
    return *this;
}

LabeledButton& LabeledButton::label_size(int size) {
    top_left.text_size(size);
    top_right.text_size(size);
    bottom_left.text_size(size);
    bottom_right.text_size(size);
    return *this;
}

LabeledButton& LabeledButton::labels(const std::string& tl, const std::string& tr, const std::string& bl, const std::string& br) {
    top_left.text(tl);
    top_right.text(tr);
    bottom_left.text(bl);
    bottom_right.text(br);
    return *this;
}

LabeledButton& LabeledButton::padding(float p) {
    button->setPosition(p, p);
    button->setSize(bindWidth(m_widget) - 2 * p, bindHeight(m_widget) - 2 * p);
    return *this;
}

Label& Label::small() { return text_size(18); }

Label& Label::medium() { return text_size(24); }

Label& Label::large() { return text_size(32); }

Label& Label::max(int val) {
    m_widget->setMaximumTextWidth(val);
    return *this;
}

Edit::Edit(const std::string& title_) {
    title(title_);
    center();
}

Edit& Edit::small() {
    title_size(18);
    return size(100, 30).text_size(18);
}

Edit& Edit::medium() {
    title_size(24);
    return size(150, 40).text_size(24);
}

Edit& Edit::large() {
    title_size(32);
    return size(200, 50).text_size(32);
}

Edit& Edit::num() {
    m_widget->setInputValidator(tgui::EditBox::Validator::UInt);
    return *this;
}

Edit& Edit::pass() {
    m_widget->setPasswordCharacter('*');
    return *this;
}

CheckBox& CheckBox::small() { return size(10, 10).text_size(18); }
CheckBox& CheckBox::medium() { return size(20, 20).text_size(24); }
CheckBox& CheckBox::large() { return size(30, 30).text_size(32); }

RadioButton& RadioButton::small() { return size(10, 10).text_size(18); }
RadioButton& RadioButton::medium() { return size(20, 20).text_size(24); }
RadioButton& RadioButton::large() { return size(30, 30).text_size(32); }

ChatBox& ChatBox::text_size(int size) {
    m_widget->setTextSize(size);
    return *this;
}

Slider& Slider::min(float value) {
    m_widget->setMinimum(value);
    return *this;
}

Slider& Slider::max(float value) {
    m_widget->setMaximum(value);
    return *this;
}

Slider& Slider::step(float value) {
    m_widget->setStep(value);
    return *this;
}

Slider& Slider::set(float value) {
    m_widget->setValue(value);
    return *this;
}

ProgressBar& ProgressBar::min(unsigned val) {
    m_widget->setMinimum(val);
    return *this;
}
ProgressBar& ProgressBar::max(unsigned val) {
    m_widget->setMaximum(val);
    return *this;
}
ProgressBar& ProgressBar::set(unsigned val) {
    m_widget->setValue(val);
    return *this;
}

Tabs& Tabs::add(std::string str) {
    m_widget->add(str);
    return *this;
}

Tabs& Tabs::select(int val) {
    m_widget->select(val);
    return *this;
}

Tabs& Tabs::tab_height(float val) {
    m_widget->setTabHeight(val);
    return *this;
}

ListBox& ListBox::add(std::string str) {
    m_widget->addItem(str);
    return *this;
}

ListBox& ListBox::remove(std::string str) {
    m_widget->removeItem(str);
    return *this;
}

ListBox& ListBox::clear() {
    m_widget->removeAllItems();
    return *this;
}

}  // namespace os
