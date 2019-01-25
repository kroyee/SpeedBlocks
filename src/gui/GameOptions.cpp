#include "GameOptions.h"
#include "GameSignals.h"
#include "Options.h"
#include "Resources.h"
using std::to_string;

#ifdef __APPLE__
#include "ResourcePath.hpp"
#else
#include "EmptyResourcePath.h"
#endif

static auto& EnableBackground = Signal<void, const sf::Time&>::get("EnableBackground");
static auto& DisableBackground = Signal<void>::get("DisableBackground");
static auto& LightTheme = Signal<void>::get("LightTheme");
static auto& DarkTheme = Signal<void>::get("DarkTheme");
static auto& SetGameBackColor = Signal<void, int>::get("SetGameBackColor");
static auto& SetDrawMe = Signal<void>::get("SetDrawMe");
static auto& SetFieldsBackColor = Signal<void, int>::get("SetFieldsBackColor");
static auto& Show = Signal<void, int>::get("Show");
static auto& Hide = Signal<void, int>::get("Hide");
static auto& SetMusicVolume = Signal<void, int>::get("SetMusicVolume");
static auto& SetEffectVolume = Signal<void, int>::get("SetEffectVolume");
static auto& SetAlertsVolume = Signal<void, int>::get("SetAlertsVolume");

GameOptions::GameOptions(sf::Rect<int> _pos, Resources& _res, os::Panel parentPanel) : GuiBase(_pos, _res, parentPanel) {
    GenOpt.size(560, 560).hide().add_to(panel);

    initSprites();

    SelectKey.size(960, 600).hide().add_to(panel);

    os::Panel box;
    box.size(300, 100).pos(330, 150).add_to(SelectKey);

    os::Label().text("Press any key").text_size(32).pos(40, 30).add_to(box);

    BindLeft.pos(100, 60).text(SFKeyToString(Options::get<sf::Keyboard::Key>("left"))).title("Left").add_to(GenOpt).connect("pressed", [&]() {
        setKey(BindLeft, Options::get<sf::Keyboard::Key>("left"));
    });

    BindRight.pos(100, 100).text(SFKeyToString(Options::get<sf::Keyboard::Key>("right"))).title("Right").add_to(GenOpt).connect("pressed", [&]() {
        setKey(BindRight, Options::get<sf::Keyboard::Key>("right"));
    });

    BindDown.pos(100, 140).text(SFKeyToString(Options::get<sf::Keyboard::Key>("Down"))).title("Down").add_to(GenOpt).connect("pressed", [&]() {
        setKey(BindDown, Options::get<sf::Keyboard::Key>("Down"));
    });

    BindRCW.pos(100, 180).text(SFKeyToString(Options::get<sf::Keyboard::Key>("rcw"))).title("Rotate >").add_to(GenOpt).connect("pressed", [&]() {
        setKey(BindRCW, Options::get<sf::Keyboard::Key>("rcw"));
    });

    BindRCCW.pos(100, 220).text(SFKeyToString(Options::get<sf::Keyboard::Key>("rccw"))).title("Rotate <").add_to(GenOpt).connect("pressed", [&]() {
        setKey(BindRCCW, Options::get<sf::Keyboard::Key>("rccw"));
    });

    BindR180.pos(350, 60).text(SFKeyToString(Options::get<sf::Keyboard::Key>("r180"))).title("Rotate").add_to(GenOpt).connect("pressed", [&]() {
        setKey(BindR180, Options::get<sf::Keyboard::Key>("r180"));
    });

    BindHD.pos(350, 100).text(SFKeyToString(Options::get<sf::Keyboard::Key>("hd"))).title("Hard").add_to(GenOpt).connect("pressed", [&]() {
        setKey(BindHD, Options::get<sf::Keyboard::Key>("hd"));
    });

    BindMenu.pos(350, 140).text(SFKeyToString(Options::get<sf::Keyboard::Key>("menu"))).title("Menu").add_to(GenOpt).connect("pressed", [&]() {
        setKey(BindMenu, Options::get<sf::Keyboard::Key>("menu"));
    });

    BindScore.pos(350, 180).text(SFKeyToString(Options::get<sf::Keyboard::Key>("score"))).title("Score").add_to(GenOpt).connect("pressed", [&]() {
        setKey(BindScore, Options::get<sf::Keyboard::Key>("score"));
    });

    BindAway.pos(350, 220).text(SFKeyToString(Options::get<sf::Keyboard::Key>("away"))).title("Away").add_to(GenOpt).connect("pressed", [&]() {
        setKey(BindAway, Options::get<sf::Keyboard::Key>("away"));
    });

    BindReady.pos(350, 260).text(SFKeyToString(Options::get<sf::Keyboard::Key>("ready"))).title("Ready").add_to(GenOpt).connect("pressed", [&]() {
        setKey(BindReady, Options::get<sf::Keyboard::Key>("ready"));
    });

    std::array<os::Button, 7> Rp;  // Align Pieces
    std::array<os::Button, 7> Cc;

    tgui::Texture rotate_n(resources.gfx->texture("rotate"));
    tgui::Texture rotate_h(resources.gfx->texture("rotate"));
    // rotate_n.setColor({0, 0, 0});

    tgui::Texture color_n(resources.gfx->texture("color"));
    tgui::Texture color_h(resources.gfx->texture("color"));
    // color_n.setColor({0, 0, 0});

    for (int i = 0; i < 7; i++) {
        Rp[i]->getRenderer()->setTexture(rotate_n);
        Rp[i]->getRenderer()->setTextureHover(rotate_h);
        Rp[i]->getRenderer()->setBorders({0, 0, 0, 0});

        Cc[i]->getRenderer()->setTexture(color_n);
        Cc[i]->getRenderer()->setTextureHover(color_h);
        Cc[i]->getRenderer()->setBorders({0, 0, 0, 0});

        Rp[i].pos(i * 80 + 10, 365).size(25, 25).connect("pressed", &GameOptions::rotPiece, this, i).add_to(GenOpt);

        Cc[i].pos(i * 80 + 10, 365).size(25, 25).connect("pressed", &GameOptions::colPiece, this, i).add_to(GenOpt);
    }

    GenOpt.add(
        os::Label().pos(50, 410).center().text("Left & Right Repeat\nDelay         Speed"),
        os::Edit().pos(50, 460).size(70, 30).num().text(to_string(Options::get<sf::Time>("repeatdelay").asMilliseconds())).connect("TextChanged", &Options::setDelay, 1),
        os::Edit().pos(150, 460).size(70, 30).num().text(to_string(Options::get<sf::Time>("repeatspeed").asMilliseconds())).connect("TextChanged", &Options::setDelay, 2),
        os::Label().pos(300, 410).center().text("Down Repeat\n  Delay         Speed"),
        os::Edit()
            .pos(300, 460)
            .size(70, 30)
            .num()
            .text(to_string(Options::get<sf::Time>("repeatdelaydown").asMilliseconds()))
            .connect("TextChanged", &Options::setDelay, 3),
        os::Edit()
            .pos(400, 460)
            .size(70, 30)
            .num()
            .text(to_string(Options::get<sf::Time>("repeatspeeddown").asMilliseconds()))
            .connect("TextChanged", &Options::setDelay, 4));

    VidOpt.size(560, 560).hide().add_to(panel);

    os::Label().pos(250, 0).text("Video Mode").add_to(VidOpt);

    auto value = (Options::get<short>("currentmode") == -1) ? 0 : Options::get<short>("currentmode");
    VMSlider.pos(50, 50)
        .size(460, 30)
        .max(Options::get_videomodes().size() - 1)
        .set(value)
        .disable()
        .add_to(VidOpt)
        .connect("ValueChanged", &GameOptions::vidSlide, this);

    std::string cvmname = to_string(Options::get_videomodes()[VMSlider->getValue()].width) + "x" + to_string(Options::get_videomodes()[VMSlider->getValue()].height);
    VideoMode.pos(240, 90).text(cvmname).add_to(VidOpt);

    Fullscreen.pos(60, 130).text("Fullscreen").add_to(VidOpt).connect({"Checked", "Unchecked"}, &GameOptions::fsChecked, this);
    if (Options::get<bool>("fullscreen")) {
        Fullscreen.check();
        VMSlider.enable();
    }

    vSync.pos(220, 130).text("VSync").add_to(VidOpt);
    if (Options::get<bool>("vsync")) vSync.check();

    performanceOutput.pos(320, 130).text("Performance output").add_to(VidOpt);
    if (Options::get<bool>("performanceOutput")) performanceOutput.check();

    os::Label().pos(80, 200).text("Frame Rate").add_to(VidOpt);

    FrameDelay.pos(100, 230).size(70, 30).num().text(to_string(1000000 / Options::get<sf::Time>("framedelay").asMicroseconds())).add_to(VidOpt);

    os::Label().pos(320, 200).text("Input Rate").add_to(VidOpt);

    InputDelay.pos(310, 230).size(110, 30).num().text(to_string(1000000 / Options::get<sf::Time>("inputdelay").asMicroseconds())).add_to(VidOpt);

    os::Button().text("Apply").pos(240, 300).connect("pressed", &GameOptions::applyVideo, this).add_to(VidOpt);

    os::Label()
        .pos(10, 390)
        .text("Frame Rate = frames drawn per second\nInput Rate = input parsed per second\nNote: Enabling vSync will disable both FrameRate and InputRate.")
        .add_to(VidOpt);

    SndOpt.size(560, 560).pos(0, 0).hide().add_to(panel);
    if (Options::get<bool>("noSound")) SndOpt.disable();

    os::CheckBox EsC;
    EsC.pos(50, 0).text("Sound Enabled").connect({"Checked", "Unchecked"}, &GameOptions::sndChecked, this).add_to(SndOpt);
    if (Options::get<bool>("sound")) EsC.check();

    os::Slider().pos(50, 100).size(460, 30).max(100).connect("ValueChanged", &GameOptions::volSlide, this, 1).set(Options::get<short>("musicvolume")).add_to(SndOpt);

    os::Label().text("Music Volume").pos(240, 50).add_to(SndOpt);

    os::Slider().pos(50, 200).size(460, 30).max(100).connect("ValueChanged", &GameOptions::volSlide, this, 2).set(Options::get<short>("effectvolume")).add_to(SndOpt);

    os::Label().text("Effect Volume").pos(240, 150).add_to(SndOpt);

    os::Slider().pos(50, 300).size(460, 30).max(100).connect("ValueChanged", &GameOptions::volSlide, this, 3).set(Options::get<short>("chatvolume")).add_to(SndOpt);

    os::Label().text("Alert Volume").pos(240, 250).add_to(SndOpt);

    VisOpt.size(560, 560).pos(0, 0).hide().add_to(panel);

    os::Label().pos(20, 13).text("Name (for single-player)").add_to(VisOpt);
    os::Edit().text(Options::get<std::string>("name")).pos(230, 10).size(270, 30).connect("TextChanged", &GameOptions::changeName, this).add_to(VisOpt);

    os::CheckBox()
        .pos(20, 50)
        .text("Show background animation")
        .connect("Checked", [&]() { EnableBackground(resources.delayClock.getElapsedTime()); })
        .connect("Unchecked", [&]() { DisableBackground(); })
        .add_to(VisOpt)
        .check(Options::get<bool>("animatedBackground"));

    os::Label().pos(100, 90).text("Ghost piece transparency").add_to(VisOpt);

    os::Slider()
        .pos(20, 120)
        .size(520, 30)
        .max(255)
        .set(Options::get<uint8_t>("ghostPieceAlpha"))
        .connect("ValueChanged", &GameOptions::setGhostPieceAlpha, this)
        .add_to(VisOpt);

    auto check = (Options::get<uint8_t>("theme") == 1);
    os::RadioButton().pos(20, 180).text("Light theme").check(check).add_to(VisOpt).connect("Checked", [&]() { LightTheme(); });

    os::RadioButton().pos(20, 220).text("Dark theme").check(!check).add_to(VisOpt).connect("Checked", [&]() { DarkTheme(); });

    os::Label().pos(100, 260).text("Game field background color and lines").add_to(VisOpt);

    os::Slider().pos(20, 290).size(520, 30).max(255).set(Options::get<uint8_t>("fieldBackground")).add_to(VisOpt).connect("ValueChanged", [&](float val) {
        Options::get<uint8_t>("fieldBackground") = val;
        SetGameBackColor(val);
        SetDrawMe();
        SetFieldsBackColor(val);
    });

    os::CheckBox()
        .pos(20, 500)
        .text("Menu responds to mouse hovering")
        .check(Options::get<bool>("mouseMenu"))
        .connect("Checked", [&]() { Options::get<bool>("mouseMenu") = true; })
        .connect("Unchecked", [&]() { Options::get<bool>("mouseMenu") = false; })
        .add_to(VisOpt);

    connectSignal("ShowOptions", &GameOptions::show, this);
}

void GameOptions::show(int index) {
    panel.show();
    GenOpt.hide();
    VidOpt.hide();
    SndOpt.hide();
    VisOpt.hide();
    if (index == 0)
        VisOpt.show();
    else if (index == 1)
        GenOpt.show();
    else if (index == 2) {
        VidOpt.show();
        if (Options::get<bool>("fullscreen"))
            Fullscreen.check();
        else
            Fullscreen.uncheck();
        if (Options::get<bool>("vsync"))
            vSync.check();
        else
            vSync.uncheck();
        FrameDelay.text(to_string(1000000 / Options::get<sf::Time>("framedelay").asMicroseconds()));
        InputDelay.text(to_string(1000000 / Options::get<sf::Time>("inputdelay").asMicroseconds()));
    } else if (index == 3)
        SndOpt.show();
}

void GameOptions::changeName(const std::string& name) {
    Options::get<std::string>("name") = name;
    // gui->game.field.text.setName(name);
}

void GameOptions::vidSlide(float i) {
    std::string name;
    name = to_string(Options::get_videomodes()[i].width) + "x" + to_string(Options::get_videomodes()[i].height);
    VideoMode.text(name);
}

void GameOptions::fsChecked(bool i) { VMSlider.enable(i); }

void GameOptions::sndChecked(bool i) { Options::get<bool>("sound") = i; }

void GameOptions::applyVideo() {
    if (Fullscreen->isChecked()) {
        if (!Options::get<bool>("fullscreen") || Options::get<short>("currentmode") != VMSlider->getValue()) {
            Options::get<bool>("fullscreen") = true;
            Options::get<short>("currentmode") = VMSlider->getValue();
            resources.window.close();
            resources.window.create(Options::get_videomodes()[Options::get<short>("currentmode")], "SpeedBlocks", sf::Style::Fullscreen);
            resources.window.setView(sf::View(sf::FloatRect(0, 0, 960, 600)));
        }
    } else if (Options::get<bool>("fullscreen")) {
        Options::get<bool>("fullscreen") = false;
        resources.window.close();
        resources.window.create(sf::VideoMode(960, 600), "SpeedBlocks");
        resources.window.setView(sf::View(sf::FloatRect(0, 0, 960, 600)));
    }

    if (vSync->isChecked()) {
        Options::get<bool>("vsync") = true;
        resources.window.setVerticalSyncEnabled(true);
    } else {
        Options::get<bool>("vsync") = false;
        resources.window.setVerticalSyncEnabled(false);
    }

    if (performanceOutput->isChecked()) {
        Options::get<bool>("performanceOutput") = true;
        Show(6);
    } else {
        Options::get<bool>("performanceOutput") = false;
        Hide(6);
    }

    std::string fd = FrameDelay->getText();
    short value = 0;
    if (fd.size()) value = stoi(fd);
    if (value) Options::get<sf::Time>("framedelay") = sf::microseconds(1000000 / value);
    value = 0;
    fd = InputDelay->getText();
    if (fd.size()) value = stoi(fd);
    if (value) Options::get<sf::Time>("inputdelay") = sf::microseconds(1000000 / value);
}

void GameOptions::volSlide(int i, float vol) {
    if (i == 1) {
        Options::get<short>("musicvolume") = vol;
        SetMusicVolume(vol);
    } else if (i == 2) {
        Options::get<short>("effectvolume") = vol;
        SetEffectVolume(vol);
    } else if (i == 3) {
        Options::get<short>("chatvolume") = vol;
        SetAlertsVolume(vol);
    }
}

void GameOptions::setKey(os::TitledButton& b, sf::Keyboard::Key& k) {
    SelectKey.show();
    key = &k;
    button = &b;
}

bool GameOptions::putKey(sf::Event& event) {
    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::Key::Escape || event.key.code == -1)
            SelectKey.hide();
        else {
            SelectKey.hide();

            if (event.key.code == Options::get<sf::Keyboard::Key>("left")) {
                Options::get<sf::Keyboard::Key>("left") = sf::Keyboard::Unknown;
                BindLeft.text("");
            }

            if (event.key.code == Options::get<sf::Keyboard::Key>("right")) {
                Options::get<sf::Keyboard::Key>("right") = sf::Keyboard::Unknown;
                BindRight.text("");
            }

            if (event.key.code == Options::get<sf::Keyboard::Key>("down")) {
                Options::get<sf::Keyboard::Key>("down") = sf::Keyboard::Unknown;
                BindDown.text("");
            }

            if (event.key.code == Options::get<sf::Keyboard::Key>("rcw")) {
                Options::get<sf::Keyboard::Key>("rcw") = sf::Keyboard::Unknown;
                BindRCW.text("");
            }

            if (event.key.code == Options::get<sf::Keyboard::Key>("rccw")) {
                Options::get<sf::Keyboard::Key>("rccw") = sf::Keyboard::Unknown;
                BindRCCW.text("");
            }

            if (event.key.code == Options::get<sf::Keyboard::Key>("r180")) {
                Options::get<sf::Keyboard::Key>("r180") = sf::Keyboard::Unknown;
                BindR180.text("");
            }

            if (event.key.code == Options::get<sf::Keyboard::Key>("hd")) {
                Options::get<sf::Keyboard::Key>("hd") = sf::Keyboard::Unknown;
                BindHD.text("");
            }

            if (event.key.code == Options::get<sf::Keyboard::Key>("menu")) {
                Options::get<sf::Keyboard::Key>("menu") = sf::Keyboard::Unknown;
                BindMenu.text("");
            }

            if (event.key.code == Options::get<sf::Keyboard::Key>("score")) {
                Options::get<sf::Keyboard::Key>("score") = sf::Keyboard::Unknown;
                BindScore.text("");
            }

            if (event.key.code == Options::get<sf::Keyboard::Key>("away")) {
                Options::get<sf::Keyboard::Key>("away") = sf::Keyboard::Unknown;
                BindAway.text("");
            }

            if (event.key.code == Options::get<sf::Keyboard::Key>("ready")) {
                Options::get<sf::Keyboard::Key>("ready") = sf::Keyboard::Unknown;
                BindReady.text("");
            }

            *key = event.key.code;
            button->text(SFKeyToString(event.key.code));
        }
        return true;
    }
    return false;
}

void GameOptions::rotPiece(short i) {
    auto& basepiece = Options::get_basepieces();

    basepiece[i].setRotation(++basepiece[i].rotation);

    piecePreview[i]->m_sprite.setRotation(basepiece[i].rotation * 90);
}

void GameOptions::colPiece(short i) {
    auto& basepiece = Options::get_basepieces();

    basepiece[i].setColor(++basepiece[i].tile);

    piecePreview[i]->m_sprite.setColor(pColor(basepiece[i].tile));
}

void GameOptions::initSprites() {
    sf::Texture tileT;
    sf::Sprite tile;
    tileT.loadFromFile(resourcePath() + "media/tile.png");
    tile.setTexture(tileT);

    sf::RenderTexture rendtex;
    rendtex.create(120, 120);

    std::vector<short> value = Options::pieceArray();

    for (int p = 0; p < 7; p++) {
        rendtex.clear(sf::Color(255, 255, 255, 0));
        if (p == 4 || p == 6) {
            for (int x = 0; x < 4; x++)
                for (int y = 0; y < 4; y++)
                    if (value[16 * p + 4 * y + x]) {
                        tile.setPosition(x * 30, y * 30);
                        rendtex.draw(tile);
                    }
        } else {
            for (int x = 0; x < 3; x++)
                for (int y = 0; y < 3; y++)
                    if (value[16 * p + 4 * y + x]) {
                        tile.setPosition(x * 30 + 15, y * 30 + 15);
                        rendtex.draw(tile);
                    }
        }
        rendtex.display();
        piecePreview[p] = tgui::TextureAndSprite::create();
        piecePreview[p]->m_texture = rendtex.getTexture();
        piecePreview[p]->m_sprite.setTexture(piecePreview[p]->m_texture);
        piecePreview[p]->m_sprite.setScale(0.5, 0.5);
        piecePreview[p]->m_sprite.setPosition(80 * p + 40, 330);
        piecePreview[p]->m_sprite.setColor(pColor(Options::get_basepieces()[p].tile));
        piecePreview[p]->m_sprite.setOrigin(60, 60);
        piecePreview[p]->m_sprite.setRotation(Options::get<uint8_t>("piece_" + std::to_string(p) + "_rotation") * 90);
        GenOpt->add(piecePreview[p]);
    }
}

sf::Color pColor(short i) {
    sf::Color col;
    switch (i) {
        case 1:
            col.r = 255;
            col.g = 0;
            col.b = 0;
            break;
        case 2:
            col.r = 0;
            col.g = 255;
            col.b = 0;
            break;
        case 3:
            col.r = 115;
            col.g = 145;
            col.b = 255;
            break;
        case 4:
            col.r = 255;
            col.g = 0;
            col.b = 255;
            break;
        case 5:
            col.r = 0;
            col.g = 255;
            col.b = 255;
            break;
        case 6:
            col.r = 255;
            col.g = 255;
            col.b = 0;
            break;
        case 7:
            col.r = 255;
            col.g = 255;
            col.b = 255;
            break;

        default:
            col.r = 0;
            col.g = 0;
            col.b = 0;
            break;
    }
    return col;
}

std::string SFKeyToString(unsigned int keycode) {
    switch (keycode) {
        case sf::Keyboard::Key::Escape:
            return "Escape";
            break;
        case sf::Keyboard::Key::LControl:
            return "LControl";
            break;
        case sf::Keyboard::Key::LShift:
            return "LShift";
            break;
        case sf::Keyboard::Key::LAlt:
            return "LAlt";
            break;
        case sf::Keyboard::Key::LSystem:
            return "LSystem";
            break;
        case sf::Keyboard::Key::RControl:
            return "RControl";
            break;
        case sf::Keyboard::Key::RShift:
            return "RShift";
            break;
        case sf::Keyboard::Key::RAlt:
            return "RAlt";
            break;
        case sf::Keyboard::Key::RSystem:
            return "RSystem";
            break;
        case sf::Keyboard::Key::Menu:
            return "Menu";
            break;
        case sf::Keyboard::Key::LBracket:
            return "LBracket";
            break;
        case sf::Keyboard::Key::RBracket:
            return "RBracket";
            break;
        case sf::Keyboard::Key::SemiColon:
            return ";";
            break;
        case sf::Keyboard::Key::Comma:
            return ",";
            break;
        case sf::Keyboard::Key::Period:
            return ".";
            break;
        case sf::Keyboard::Key::Quote:
            return "\'";
            break;
        case sf::Keyboard::Key::Slash:
            return "/";
            break;
        case sf::Keyboard::Key::BackSlash:
            return "\\";
            break;
        case sf::Keyboard::Key::Tilde:
            return "~";
            break;
        case sf::Keyboard::Key::Equal:
            return "=";
            break;
        case sf::Keyboard::Key::Dash:
            return "-";
            break;
        case sf::Keyboard::Key::Space:
            return "Space";
            break;
        case sf::Keyboard::Key::Return:
            return "Return";
            break;
        case sf::Keyboard::Key::Tab:
            return "Tab";
            break;
        case sf::Keyboard::Key::PageUp:
            return "Page Up";
            break;
        case sf::Keyboard::Key::PageDown:
            return "Page Down";
            break;
        case sf::Keyboard::Key::End:
            return "End";
            break;
        case sf::Keyboard::Key::Home:
            return "Home";
            break;
        case sf::Keyboard::Key::Insert:
            return "Insert";
            break;
        case sf::Keyboard::Key::Delete:
            return "Delete";
            break;
        case sf::Keyboard::Key::Add:
            return "+";
            break;
        case sf::Keyboard::Key::Subtract:
            return "-";
            break;
        case sf::Keyboard::Key::Multiply:
            return "*";
            break;
        case sf::Keyboard::Key::Divide:
            return "/";
            break;
        case sf::Keyboard::Key::Left:
            return "Left";
            break;
        case sf::Keyboard::Key::Right:
            return "Right";
            break;
        case sf::Keyboard::Key::Up:
            return "Up";
            break;
        case sf::Keyboard::Key::Down:
            return "Down";
            break;
        case sf::Keyboard::Key::BackSpace:
            return "BackSpace";
            break;
        case sf::Keyboard::Key::Numpad0:
            return "NP 0";
            break;
        case sf::Keyboard::Key::Numpad1:
            return "NP 1";
            break;
        case sf::Keyboard::Key::Numpad2:
            return "NP 2";
            break;
        case sf::Keyboard::Key::Numpad3:
            return "NP 3";
            break;
        case sf::Keyboard::Key::Numpad4:
            return "NP 4";
            break;
        case sf::Keyboard::Key::Numpad5:
            return "NP 5";
            break;
        case sf::Keyboard::Key::Numpad6:
            return "NP 6";
            break;
        case sf::Keyboard::Key::Numpad7:
            return "NP 7";
            break;
        case sf::Keyboard::Key::Numpad8:
            return "NP 8";
            break;
        case sf::Keyboard::Key::Numpad9:
            return "NP 9";
            break;
        case sf::Keyboard::Key::Num1:
            return "1";
            break;
        case sf::Keyboard::Key::Num2:
            return "2";
            break;
        case sf::Keyboard::Key::Num3:
            return "3";
            break;
        case sf::Keyboard::Key::Num4:
            return "4";
            break;
        case sf::Keyboard::Key::Num5:
            return "5";
            break;
        case sf::Keyboard::Key::Num6:
            return "6";
            break;
        case sf::Keyboard::Key::Num7:
            return "7";
            break;
        case sf::Keyboard::Key::Num8:
            return "8";
            break;
        case sf::Keyboard::Key::Num9:
            return "9";
            break;
        case sf::Keyboard::Key::Num0:
            return "0";
            break;
        case sf::Keyboard::Key::F1:
            return "F1";
            break;
        case sf::Keyboard::Key::F2:
            return "F2";
            break;
        case sf::Keyboard::Key::F3:
            return "F3";
            break;
        case sf::Keyboard::Key::F4:
            return "F4";
            break;
        case sf::Keyboard::Key::F5:
            return "F5";
            break;
        case sf::Keyboard::Key::F6:
            return "F6";
            break;
        case sf::Keyboard::Key::F7:
            return "F7";
            break;
        case sf::Keyboard::Key::F8:
            return "F8";
            break;
        case sf::Keyboard::Key::F9:
            return "F9";
            break;
        case sf::Keyboard::Key::F10:
            return "F10";
            break;
        case sf::Keyboard::Key::F11:
            return "F11";
            break;
        case sf::Keyboard::Key::F12:
            return "F12";
            break;
        case sf::Keyboard::Key::F13:
            return "F13";
            break;
        case sf::Keyboard::Key::F14:
            return "F14";
            break;
        case sf::Keyboard::Key::F15:
            return "F15";
            break;
        case sf::Keyboard::Key::Pause:
            return "Pause";
            break;

        default:
            if (keycode < 26)
                return std::string{(char)(keycode + 65)};
            else
                return "";
    }
}

void GameOptions::setGhostPieceAlpha(float alpha) {
    Options::get<uint8_t>("ghostPieceAlpha") = alpha;
    resources.gfx->setGhostPieceAlpha(alpha);
    SetDrawMe();
}
