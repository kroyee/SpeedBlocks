#ifndef TRAININGUI_H
#define TRAININGUI_H

#include "GuiBase.h"

class Resources;

class TrainingUI : public GuiBase {
   public:
    os::Label amount_text;
    os::Slider amount_slider;
    os::Label speed_text;
    os::Slider speed_slider;
    os::Label skill_text;
    os::Slider skill_slider;
    os::Label playstyle_text;
    os::Slider playstyle_slider;

    TrainingUI(sf::Rect<int> _pos, Resources& _res);
};

#endif