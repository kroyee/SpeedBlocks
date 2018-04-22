#ifndef PERFORMANCEOUTPUT_H
#define PERFORMANCEOUTPUT_H

#include "GuiBase.h"

class Resources;

class PerformanceOutput : public GuiBase {
public:
	tgui::Label::Ptr longest;
	tgui::Label::Ptr longestHeader;
	tgui::Label::Ptr input;
	tgui::Label::Ptr inputHeader;
	tgui::Label::Ptr frame;
	tgui::Label::Ptr frameHeader;
	tgui::Label::Ptr ping;
	tgui::Label::Ptr pingHeader;

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