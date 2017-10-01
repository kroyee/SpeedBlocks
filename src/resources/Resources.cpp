#include "Resources.h"
#include "textures.h"
#include "network.h"
#include "sounds.h"
#include "optionSet.h"
#include "packetcompress.h"
#include <TGUI/TGUI.hpp>

bool loadError(sf::String error) {
    if (error == "OK")
        return false;
    sf::RenderWindow window;
    window.create(sf::VideoMode(500, 400), "SpeedBlocks");
    window.clear();
    tgui::Gui gui(window);
    tgui::Label::Ptr errorMsg = tgui::Label::create();
    errorMsg->setText("Failed to load resources " + error + "\nSee that the file is there or reinstall the game.\n\nPress any key to quit.");
    errorMsg->setTextColor(sf::Color::White);
    errorMsg->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Center);
    errorMsg->setTextSize(35);
    errorMsg->setSize(500,400);
    gui.add(errorMsg);
    gui.draw();
    window.display();

    sf::Event event;
    while (1) {
        window.waitEvent(event);
        if (event.type == sf::Event::KeyPressed || event.type == sf::Event::Closed)
            break;
    }
    return true;
}

Resources::Resources(sf::RenderWindow& _window) : options(new optionSet), gfx(new textures(_window)),
sounds(new soundBank(options->sound)), net(new network), compressor(new PacketCompress), window(_window),
playonline(false), away(false), restart(false), chatFocused(false) {
    version_major = 0;
    version_minor = 1;
    version_patch = 10;
    clientVersion = version_major*10000 + version_minor*100 + version_patch;
}

Resources::~Resources() {
    delete options;
    delete gfx;
    delete sounds;
    delete net;
    delete compressor;
}

bool Resources::init() {
    if (loadError(gfx->loadTextures()))
        return false;
    if (!options->noSound) {
        if (loadError(sounds->loadSounds()))
            return false;
        sounds->setEffectVolume(options->EffectVolume);
        sounds->setMusicVolume(options->MusicVolume);
        sounds->setAlertVolume(options->ChatVolume);
    }
    else
        options->sound=false;

    gamestate = GameStates::MainMenu;

    gfx->setGhostPieceAlpha(options->ghostPieceAlpha);

    return true;
}