#ifndef GUIBASE_H
#define GUIBASE_H

#include <TGUI/TGUI.hpp>
#include <type_traits>
#include "GuiAlign.h"
#include "Textures.h"

class Resources;

namespace tgui {
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
			tgui::Panel::Ptr panel;
			Resources& resources;

			GuiBase(sf::Rect<int> _pos, Resources& _res);
			GuiBase(sf::Rect<int> _pos, Resources& _res, tgui::Panel::Ptr parentPanel);
			virtual ~GuiBase();
			virtual void show() { panel->show(); }
			virtual void hide() { panel->hide(); }
			void enable() { panel->enable(); }
			void disable() { panel->disable(); }
			bool isVisible() { return panel->isVisible(); }

			bool mouseOver(tgui::Widget::Ptr widget, int x, int y);
			bool mouseOver(tgui::Widget::Ptr widget);

			tgui::WidgetConverter loadWidget(const std::string& widget, const PosAndSize& pos = PosAndSize(), const std::string& text="");
			tgui::WidgetConverter loadWidgetTo(tgui::Panel::Ptr panel, const std::string& widget, const PosAndSize& pos = PosAndSize(), const std::string& text="");
			tgui::Panel::Ptr loadPanel(const PosAndSize& pos = PosAndSize(), bool hide=true);
			tgui::Panel::Ptr loadPanelTo(tgui::Panel::Ptr toPanel, const PosAndSize& pos = PosAndSize(), bool hide=true);

			Label::Ptr label1(const std::string& text, int size=18);
			Label::Ptr label2(const std::string& text);
			Label::Ptr label3(const std::string& text);
			Label::Ptr label4(const std::string& text);

			Button::Ptr button1(const std::string& text, sf::Vector2f size = sf::Vector2f(100, 30));
			Button::Ptr button2(const std::string& text);
			Button::Ptr button3(const std::string& text);

			EditBox::Ptr edit1(const sf::Vector2f& size = sf::Vector2f());
			EditBox::Ptr edit2(const sf::Vector2f& size = sf::Vector2f());
			EditBox::Ptr editNum1(const sf::Vector2f& size = sf::Vector2f());
			EditBox::Ptr editNum2(const sf::Vector2f& size = sf::Vector2f());
			EditBox::Ptr password(const sf::Vector2f& size = sf::Vector2f());

			TextBox::Ptr text(const sf::Vector2f& size = sf::Vector2f());
			ListBox::Ptr list(const sf::Vector2f& size = sf::Vector2f());
			ChatBox::Ptr chat(const sf::Vector2f& size = sf::Vector2f());
			CheckBox::Ptr checkbox(const std::string& text);
			RadioButton::Ptr radio(const std::string& text);
			Slider::Ptr slider(const std::string& text = "", const sf::Vector2f& size = sf::Vector2f(), int textsize = 18);
			Slider::Ptr slider(const std::string& text, int textsize);

			Panel::Ptr panel1(const PosAndSize& pos = PosAndSize(), bool hide=true);
			Panel::Ptr panel2(const PosAndSize& pos = PosAndSize(), bool hide=true);

			template <typename... T>
			Align<T...> align(int x, int y, int gap=0, bool down = true) {
				return Align<T...>(panel, x, y, gap, down);
			}

			template <typename... T>
			static Align<T...> alignOn(Panel::Ptr& panel, int x, int y, int gap=0, bool down = true) {
				return Align<T...>(panel, x, y, gap, down);
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
	}
}

using GuiBase = tgui::SBGui::GuiBase;

#endif
