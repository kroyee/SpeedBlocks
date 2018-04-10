#ifndef GAMEOPTIONS_H
#define GAMEOPTIONS_H

#include "guiBase.h"
#include "TextureAndSprite.h"

class Resources;

class GameOptions : public guiBase {
public:
	tgui::Tab::Ptr otab;
	tgui::Panel::Ptr GenOpt;
	tgui::Panel::Ptr VidOpt;
	tgui::Panel::Ptr SndOpt;
	tgui::Panel::Ptr VisOpt;

	tgui::CheckBox::Ptr Fullscreen;
	tgui::CheckBox::Ptr vSync;
	tgui::CheckBox::Ptr performanceOutput;
	tgui::EditBox::Ptr FrameDelay;
	tgui::EditBox::Ptr InputDelay;
	tgui::Label::Ptr VideoMode;
	tgui::Slider::Ptr VMSlider;
	tgui::Panel::Ptr SelectKey;

	tgui::Button::Ptr BindLeft;
	tgui::Button::Ptr BindRight;
	tgui::Button::Ptr BindDown;
	tgui::Button::Ptr BindRCW;
	tgui::Button::Ptr BindRCCW;
	tgui::Button::Ptr BindR180;
	tgui::Button::Ptr BindHD;
	tgui::Button::Ptr BindMenu;
	tgui::Button::Ptr BindScore;
	tgui::Button::Ptr BindAway;
	tgui::Button::Ptr BindReady;

	sf::Keyboard::Key* key;
	tgui::Button::Ptr button;

	tgui::TextureAndSprite::Ptr piecePreview[7];

	GameOptions(sf::Rect<int> _pos, Resources& _res, tgui::Panel::Ptr parentPanel);
	void show(int index);
	void changeName(const std::string& name);
	void vidSlide(short i);
	void fsChecked(bool i);
	void sndChecked(bool i);
	void applyVideo();
	void volSlide(short i, short vol);
	void setKey(tgui::Button::Ptr _button, sf::Keyboard::Key& _key);
	bool putKey(sf::Event& event);

	void rotPiece(short i);
	void colPiece(short i);
	void initSprites();

	void setGhostPieceAlpha(uint8_t alpha);
};

std::string SFKeyToString(unsigned int keycode);
sf::Color pColor(short i);

#endif