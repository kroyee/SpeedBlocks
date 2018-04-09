#include "textures.h"
#include <TGUI/TGUI.hpp>

#ifdef __APPLE__
#include "ResourcePath.hpp"
#else
#include "EmptyResourcePath.h"
#endif

textures::textures(sf::RenderWindow& window) : tGui(window) {}

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

    if (!typewriter.loadFromFile(resourcePath() + "media/Kingthings Trypewriter 2.ttf"))
        return "media/Kingthings Trypewriter 2.ttf";
    if (!printFont.loadFromFile(resourcePath() + "media/F25_Bank_Printer.ttf"))
        return "media/F25_Bank_Printer.ttf";
    if (!standard.loadFromFile(resourcePath() + "media/standard.ttf"))
        return "media/standard.ttf";

    icon = new sf::Image;
    if (!icon->loadFromFile(resourcePath() + "media/icon.png"))
        return "media/icon.png";

    if (!menuBackground_light.loadFromFile(resourcePath() + "media/menubackground_light.png"))
        return "media/menubackground_light.png";

    if (!menuBackground_dark.loadFromFile(resourcePath() + "media/menubackground_dark.png"))
        return "media/menubackground_dark.png";

    if (!background_light.loadFromFile(resourcePath() + "media/background_light.png"))
        return "media/background_light.png";

    if (!background_dark.loadFromFile(resourcePath() + "media/background_dark.png"))
        return "media/background_dark.png";

    if (!alert.loadFromFile(resourcePath() + "media/alert.png"))
        return "media/alert.png";

    //sf::Texture text;
    if (!rotate.loadFromFile(resourcePath() + "media/rotate.png"))
        return "media/rotate.png";
    rotate.setSmooth(true);

    if (!color.loadFromFile(resourcePath() + "media/choosecolor.png"))
        return "media/choosecolor.png";
    color.setSmooth(true);

    if (!logoTexture.loadFromFile(resourcePath() + "media/blackwhitelogo.png"))
        return "media/blackwhitelogo.png";
    logo.setTexture(logoTexture);

    gameFieldTextColor = sf::Color(0,0,0,255);

    themeTG = tgui::Theme::create(resourcePath() + "media/TransparentGrey.txt");
    tgui::Label::Ptr test;
    try {
        test = themeTG->load("Label");
    } catch (tgui::Exception) {
        return "media/TransparentGrey.txt";
    }

    tGui.setFont(standard);
    
    return "OK";
}

void textures::setGhostPieceAlpha(uint8_t alpha) {
    tile[8].setColor(sf::Color(255,0,0,alpha));
    tile[9].setColor(sf::Color(0,255,0,alpha));
    tile[10].setColor(sf::Color(115,145,255,alpha));
    tile[11].setColor(sf::Color(255,0,255,alpha));
    tile[12].setColor(sf::Color(0,255,255,alpha));
    tile[13].setColor(sf::Color(255,255,0,alpha));
    tile[14].setColor(sf::Color(255,255,255,alpha));
    tile[15].setColor(sf::Color(170,170,170,alpha));
}