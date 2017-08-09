#ifndef GAMEOPTIONS_H
#define GAMEOPTIONS_H

#include "guiBase.h"

class UI;

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
	tgui::Button::Ptr BindChat;
	tgui::Button::Ptr BindScore;
	tgui::Button::Ptr BindAway;
	tgui::Button::Ptr BindReady;

	sf::Keyboard::Key* key;
	tgui::Button::Ptr button;

	sf::Texture texture[7];
	sf::Sprite piece[7];
	tgui::Canvas::Ptr canvas[7];

	void create(sf::Rect<int> _pos, UI* _gui, tgui::Panel::Ptr parentPanel);
	void show(sf::Uint8 index);
	void changeName(const sf::String& name);
	void vidSlide(short i);
	void fsChecked(bool i);
	void sndChecked(bool i);
	void applyVideo();
	void volSlide(short i, short vol);
	void setKey(tgui::Button::Ptr _button, sf::Keyboard::Key& _key);
	void putKey(sf::Event& event);

	void rotPiece(short i);
	void colPiece(short i);
	void initSprites();

	void setGhostPieceAlpha(sf::Uint8 alpha);
};

sf::String SFKeyToString(unsigned int keycode);
sf::Color pColor(short i);

#endif