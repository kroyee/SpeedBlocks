#include "GuiBase.h"
#include "Resources.h"

namespace tgui {
	namespace SBGui {

GuiBase::GuiBase(sf::Rect<int> _pos, Resources& _res) : resources(_res) {
	panel = Panel::create();
	panel->setPosition(_pos.left, _pos.top);
	panel->setSize(_pos.width, _pos.height);
	panel->setBackgroundColor(sf::Color(255,255,255,0));
	panel->hide();
	resources.gfx->tGui.add(panel);
}

GuiBase::GuiBase(sf::Rect<int> _pos, Resources& _res, tgui::Panel::Ptr parentPanel) : resources(_res) {
	panel = loadPanelTo(parentPanel, _pos);
}

GuiBase::~GuiBase() {}

bool GuiBase::mouseOver(tgui::Widget::Ptr widget, int x, int y) {
	sf::Vector2f pos = widget->getAbsolutePosition();
	sf::Vector2f size = widget->getSize();
	if (x>=pos.x && x<=pos.x+size.x)
		if (y>=pos.y && y<=pos.y+size.y)
			return true;
	return false;
}

bool GuiBase::mouseOver(tgui::Widget::Ptr widget) {
	sf::Vector2f pos = widget->getAbsolutePosition();
	sf::Vector2f size = widget->getSize();
	sf::Vector2f mpos = resources.window.mapPixelToCoords(sf::Mouse::getPosition(resources.window));
	if (mpos.x>=pos.x-2 && mpos.x<=pos.x+size.x+2)
		if (mpos.y>=pos.y-2 && mpos.y<=pos.y+size.y+2)
			return true;
	return false;
}

tgui::WidgetConverter GuiBase::loadWidget(const std::string& widget, const PosAndSize& pos, const std::string& text) {
	return loadWidgetTo(panel, widget, pos, text);
}

tgui::WidgetConverter GuiBase::loadWidgetTo(tgui::Panel::Ptr _panel, const std::string& widget, const PosAndSize& pos, const std::string& text) {
	tgui::WidgetConverter wcnv = resources.gfx->load(widget);
	tgui::Widget::Ptr wptr = wcnv;
	wptr->setPosition(pos.x, pos.y);
	if (pos.w || pos.h)
		wptr->setSize(pos.w, pos.h);

	if (text != "") {
		if (wptr->getWidgetType() == "Button")
			static_cast<tgui::Button*>(wptr.get())->setText(text);
		else if (wptr->getWidgetType() == "Label")
			static_cast<tgui::Label*>(wptr.get())->setText(text);
	}

	_panel->add(wptr);
	return wcnv;
}

tgui::Panel::Ptr GuiBase::loadPanel(const PosAndSize& pos, bool hide) {
	return loadPanelTo(panel, pos, hide);
}

tgui::Panel::Ptr GuiBase::loadPanelTo(tgui::Panel::Ptr toPanel, const PosAndSize& pos, bool hide) {
	tgui::Panel::Ptr pan = tgui::Panel::create();
	pan->setPosition(pos.x, pos.y);
	pan->setSize(pos.w, pos.h);
	pan->setBackgroundColor(sf::Color(255,255,255,0));

	toPanel->add(pan);
	if (hide)
		pan->hide();
	return pan;
}

//
//		LABELS
//

Label::Ptr GuiBase::label1(const std::string& text, int size) {
	Label::Ptr label = resources.gfx->load("Label", text);
	label->setTextSize(size);
	return label;
}

Label::Ptr GuiBase::label2(const std::string& text) {
	return label1(text, 10);
}

Label::Ptr GuiBase::label3(const std::string& text) {
	return label1(text, 32);
}

Label::Ptr GuiBase::label4(const std::string& text) {
	return label1(text, 64);
}

//
//		BUTTONS
//

Button::Ptr GuiBase::button1(const std::string& text, sf::Vector2f size) {
	return resources.gfx->load("Button", text, size);
}

Button::Ptr GuiBase::button2(const std::string& text) {
	return resources.gfx->load("Button", text, {200, 55});
}

Button::Ptr GuiBase::button3(const std::string& text) {
	Button::Ptr b = resources.gfx->load("BigButton", text);
	b->setSize(300,100);
	return b;
}

//
//		EDITBOXES
//

EditBox::Ptr GuiBase::edit1(const sf::Vector2f& size) {
	return resources.gfx->load("EditBox", size);
}

EditBox::Ptr GuiBase::edit2(const sf::Vector2f& size) {
	EditBox::Ptr edit = resources.gfx->load("EditBox", size);
	edit->setAlignment(EditBox::Alignment::Center);
	return edit;
}

EditBox::Ptr GuiBase::editNum1(const sf::Vector2f& size) {
	EditBox::Ptr edit = resources.gfx->load("EditBox", size);
	edit->setInputValidator(EditBox::Validator::UInt);
	return edit;
}

EditBox::Ptr GuiBase::editNum2(const sf::Vector2f& size) {
	EditBox::Ptr edit = resources.gfx->load("EditBox", size);
	edit->setAlignment(EditBox::Alignment::Center);
	edit->setInputValidator(EditBox::Validator::UInt);
	return edit;
}

EditBox::Ptr GuiBase::password(const sf::Vector2f& size) {
	EditBox::Ptr edit = resources.gfx->load("EditBox", size);
	edit->setAlignment(EditBox::Alignment::Center);
	edit->setPasswordCharacter('*');
	return edit;
}

//
//		PANELS
//

Panel::Ptr GuiBase::panel1(const PosAndSize& pos, bool hide) {
	Panel::Ptr panel = Panel::create();
	panel->setPosition(pos.x, pos.y);
	panel->setSize(pos.w, pos.h);
	if (hide)
		panel->hide();
	return panel;
}

Panel::Ptr GuiBase::panel2(const PosAndSize& pos, bool hide) {
	Panel::Ptr panel = resources.gfx->load("Panel");
	tgui::Picture::Ptr pic = tgui::Picture::create(resources.gfx->guiTexture("panel"));
	pic->setSize(tgui::bindSize(panel));
	panel->add(pic);
	panel->setPosition(pos.x, pos.y);
	panel->setSize(pos.w, pos.h);
	if (hide)
		panel->hide();
	return panel;
}

//
// MISC
//

TextBox::Ptr GuiBase::text(const sf::Vector2f& size) {
	return resources.gfx->load("TextBox", size);
}

ListBox::Ptr GuiBase::list(const sf::Vector2f& size) {
	return resources.gfx->load("ListBox", size);
}

ChatBox::Ptr GuiBase::chat(const sf::Vector2f& size) {
	return resources.gfx->load("ChatBox", size);
}

CheckBox::Ptr GuiBase::checkbox(const std::string& text) {
	return resources.gfx->load("CheckBox", text);
}

RadioButton::Ptr GuiBase::radio(const std::string& text) {
	return resources.gfx->load("RadioButton", text);
}

Slider::Ptr GuiBase::slider(const std::string& text, const sf::Vector2f& size, int textsize) {
	Slider::Ptr slide = resources.gfx->load("Slider", size);
	if (text != "") {
		auto slidetext = label1("text", textsize);
		auto sliderWidth = bindWidth(slide);
		auto sliderpos = bindPosition(slide);
		slidetext->setHorizontalAlignment(Label::HorizontalAlignment::Center);
		slidetext->setPosition(sliderpos.x, sliderpos.y - 30);
		slidetext->setSize(sliderWidth, slidetext->getSize().y);
	}
	return slide;
}

Slider::Ptr GuiBase::slider(const std::string& text, int textsize) {
	return slider(text, sf::Vector2f(), textsize);
}

}}
