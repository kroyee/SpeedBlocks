#include "textures.h"
#include <SFML/Graphics.hpp>
#include <TGUI/TGUI.hpp>

#ifdef __APPLE__
#include "ResourcePath.hpp"
#else
#include "EmptyResourcePath.h"
#endif

sf::String textures::loadTextures() {
    if (!tileTexture.loadFromFile(resourcePath() + "media/tile.png"))
        return "media/tile.png";
    tileTexture.setSmooth(true);

    tile[0].setTexture(tileTexture); tile[0].setColor(sf::Color(255,0,0));
    tile[1].setTexture(tileTexture); tile[1].setColor(sf::Color(0,255,0));
    tile[2].setTexture(tileTexture); tile[2].setColor(sf::Color(115,145,255));
    tile[3].setTexture(tileTexture); tile[3].setColor(sf::Color(255,0,255));
    tile[4].setTexture(tileTexture); tile[4].setColor(sf::Color(0,255,255));
    tile[5].setTexture(tileTexture); tile[5].setColor(sf::Color(255,255,0));
    tile[6].setTexture(tileTexture); tile[6].setColor(sf::Color(255,255,255));
    tile[7].setTexture(tileTexture); tile[7].setColor(sf::Color(170,170,170));

    tile[8].setTexture(tileTexture); tile[8].setColor(sf::Color(255,0,0,120));
    tile[9].setTexture(tileTexture); tile[9].setColor(sf::Color(0,255,0,120));
    tile[10].setTexture(tileTexture); tile[10].setColor(sf::Color(115,145,255,120));
    tile[11].setTexture(tileTexture); tile[11].setColor(sf::Color(255,0,255,120));
    tile[12].setTexture(tileTexture); tile[12].setColor(sf::Color(0,255,255,120));
    tile[13].setTexture(tileTexture); tile[13].setColor(sf::Color(255,255,0,120));
    tile[14].setTexture(tileTexture); tile[14].setColor(sf::Color(255,255,255,120));
    tile[15].setTexture(tileTexture); tile[15].setColor(sf::Color(170,170,170,120));

    if (!fieldBackgroundTexture.loadFromFile(resourcePath() + "media/fieldback.png"))
        return "media/fieldback.png";
    fieldBackgroundTexture.setSmooth(true);

    fieldBackground.setTexture(fieldBackgroundTexture);

    if (!backgroundTexture.loadFromFile(resourcePath() + "media/background.png"))
        return "media/background.png";
    background.setTexture(backgroundTexture);

    if (!typewriter.loadFromFile(resourcePath() + "media/Kingthings Trypewriter 2.ttf"))
        return "media/Kingthings Trypewriter 2.ttf";
    if (!printFont.loadFromFile(resourcePath() + "media/F25_Bank_Printer.ttf"))
        return "media/F25_Bank_Printer.ttf";
    
    return "OK";
}

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

    sounds.setEffectVolume(options.EffectVolume);
    sounds.setMusicVolume(options.MusicVolume);
    sounds.setChatVolume(options.ChatVolume);

    return true;
}