#ifndef PERFORMANCEOUTPUT_H
#define PERFORMANCEOUTPUT_H

#include "GuiBase.h"

class Resources;

class PerformanceOutput : public GuiBase {
   public:
    os::Label longest;
    os::Label longestHeader;
    os::Label input;
    os::Label inputHeader;
    os::Label frame;
    os::Label frameHeader;
    os::Label ping;
    os::Label pingHeader;

    bool pingReturned;
    uint8_t pingIdCount;
    sf::Time pingTime;

    sf::Time longestFrame, secCount;
    int frameCount, frameRate;

    PerformanceOutput(sf::Rect<int> _pos, Resources& _res);

    void update(sf::Time current, sf::Time lastFrame);

    void setFrameRate(int fr);
    void setInputRate(int fc);
    void setLongestFrame(sf::Time& lf);
    void setPing(int pingResult);
};

#endif