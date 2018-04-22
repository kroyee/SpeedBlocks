#ifndef CONNECTING_H
#define CONNECTING_H

#include "GuiBase.h"

class Resources;

class Connecting : public GuiBase {
public:
	tgui::Label::Ptr label, progressLabel;
	tgui::TextBox::Ptr changelog;
	tgui::Button::Ptr cancel, apply;
	tgui::ProgressBar::Ptr downloadProgress;
	Connecting(sf::Rect<int> _pos, Resources& _res);

	void setText(const std::string& text);
	void setDownloadProgress(int64_t total, int64_t so_far);
};

#endif
