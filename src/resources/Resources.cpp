#include "Resources.h"
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

bool Resources::init() {
    if (loadError(gfx.loadTextures()))
        return false;
    if (loadError(sounds.loadSounds()))
        return false;

    gamestate = MainMenu;

    sounds.setEffectVolume(options.EffectVolume);
    sounds.setMusicVolume(options.MusicVolume);
    sounds.setChatVolume(options.ChatVolume);

    return true;
}