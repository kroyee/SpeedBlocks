#include "Resources.h"
#include <TGUI/TGUI.hpp>
#include "Options.h"
#include "Textures.h"
#include "network.h"
#include "sounds.h"

bool loadError(std::string error) {
    if (error == "OK") return false;
    sf::RenderWindow window;
    window.create(sf::VideoMode(500, 400), "SpeedBlocks");
    window.clear();
    tgui::Gui gui(window);
    tgui::Label::Ptr errorMsg = tgui::Label::create();
    errorMsg->setText("Failed to load resources " + error + "\nSee that the file is there or reinstall the game.\n\nPress any key to quit.");
    errorMsg->getRenderer()->setTextColor(sf::Color::White);
    errorMsg->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Center);
    errorMsg->setTextSize(35);
    errorMsg->setSize(500, 400);
    gui.add(errorMsg);
    gui.draw();
    window.display();

    sf::Event event;
    while (1) {
        window.waitEvent(event);
        if (event.type == sf::Event::KeyPressed || event.type == sf::Event::Closed) break;
    }
    return true;
}

Resources::Resources(sf::RenderWindow& _window)
    : gfx(new Textures(_window)), sounds(new soundBank), net(new network), window(_window), playonline(false), away(false), restart(false), chatFocused(false) {
    version_major = 0;
    version_minor = 1;
    version_patch = 14;
    clientVersion = version_major * 10000 + version_minor * 100 + version_patch;
}

Resources::~Resources() {
    delete gfx;
    delete sounds;
    delete net;
}

bool Resources::init() {
    Options::load();

    if (loadError(gfx->loadTextures(window))) return false;
    if (!Options::get<bool>("noSound")) {
        if (loadError(sounds->loadSounds())) return false;
        sounds->setEffectVolume(Options::get<short>("effectvolume"));
        sounds->setMusicVolume(Options::get<short>("musicvolume"));
        sounds->setAlertVolume(Options::get<short>("chatvolume"));
    } else
        Options::get<bool>("sound") = false;

    gamestate = GameStates::MainMenu;

    gfx->setGhostPieceAlpha(Options::get<uint8_t>("ghostPieceAlpha"));

    return true;
}
