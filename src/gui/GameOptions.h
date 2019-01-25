#ifndef GAMEOPTIONS_H
#define GAMEOPTIONS_H

#include "GuiBase.h"
#include "TextureAndSprite.h"

class Resources;

class GameOptions : public GuiBase {
   public:
    os::Panel GenOpt;
    os::Panel VidOpt;
    os::Panel SndOpt;
    os::Panel VisOpt;

    os::CheckBox Fullscreen;
    os::CheckBox vSync;
    os::CheckBox performanceOutput;
    os::Edit FrameDelay;
    os::Edit InputDelay;
    os::Label VideoMode;
    os::Slider VMSlider;
    os::Panel SelectKey;

    os::TitledButton BindLeft;
    os::TitledButton BindRight;
    os::TitledButton BindDown;
    os::TitledButton BindRCW;
    os::TitledButton BindRCCW;
    os::TitledButton BindR180;
    os::TitledButton BindHD;
    os::TitledButton BindMenu;
    os::TitledButton BindScore;
    os::TitledButton BindAway;
    os::TitledButton BindReady;

    sf::Keyboard::Key* key;
    os::TitledButton* button;

    tgui::TextureAndSprite::Ptr piecePreview[7];

    GameOptions(sf::Rect<int> _pos, Resources& _res, os::Panel parentPanel);
    void show(int index);
    void changeName(const std::string& name);
    void vidSlide(float i);
    void fsChecked(bool i);
    void sndChecked(bool i);
    void applyVideo();
    void volSlide(int i, float vol);
    void setKey(os::TitledButton& _button, sf::Keyboard::Key& _key);
    bool putKey(sf::Event& event);

    void rotPiece(short i);
    void colPiece(short i);
    void initSprites();

    void setGhostPieceAlpha(float alpha);
};

std::string SFKeyToString(unsigned int keycode);
sf::Color pColor(short i);

#endif