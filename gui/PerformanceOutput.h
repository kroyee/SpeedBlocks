#ifndef PERFORMANCEOUTPUT_H
#define PERFORMANCEOUTPUT_H

#include "guiBase.h"

class UI;

class PerformanceOutput : public guiBase {
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
	sf::Uint8 pingIdCount;
	sf::Time pingTime;

	void create(sf::Rect<int> _pos, UI* _ui);

	void setFrameRate(int fr);
	void setInputRate(int fc);
	void setLongestFrame(sf::Time& lf);
	void setPing(int pingResult);
};

#endif