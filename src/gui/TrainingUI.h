#ifndef TRAININGUI_H
#define TRAININGUI_H

#include "GuiBase.h"

class Resources;

class TrainingUI : public GuiBase {
public:
	tgui::Label::Ptr amount_text;
	tgui::Slider::Ptr amount_slider;
	tgui::Label::Ptr speed_text;
	tgui::Slider::Ptr speed_slider;
	tgui::Label::Ptr skill_text;
	tgui::Slider::Ptr skill_slider;
	tgui::Label::Ptr playstyle_text;
	tgui::Slider::Ptr playstyle_slider;

	TrainingUI(sf::Rect<int> _pos, Resources& _res);
};

#endif