#include "Textures.h"
#include <TGUI/TGUI.hpp>
#include <iostream>
#include "TguiWidgets.hpp"

#ifdef __APPLE__
#include "ResourcePath.hpp"
#else
#include "EmptyResourcePath.h"
#endif

template <typename T>
std::map<std::string, T>& getMap() {
    static std::map<std::string, T> map;
    return map;
}

template <typename T>
std::string loadResource(std::string name, std::string filename) {
    auto& map = getMap<T>();
    if (map.find(name) != map.end()) return {"Resource " + name + " already loaded"};

    if (!map[name].loadFromFile(resourcePath() + filename)) return filename;

    return "";
}

template <typename T>
std::string loadResourceList(std::vector<std::pair<std::string, std::string>> names) {
    for (auto& name : names) {
        auto msg = loadResource<T>(name.first, name.second);
        if (msg != "") return msg;
    }

    return "";
}

std::string loadThemeResources() {
    sf::Texture sheet;
    if (!sheet.loadFromFile("media/neutralUIsheet.png")) return "media/neutralUIsheet.png";

    sheet.setSmooth(true);

    auto& map = getMap<tgui::Texture>();

    map["panel"] = tgui::Texture(sheet, {240, 0, 250, 250}, {30, 30, 190, 190});
    map["buttonDown"] = tgui::Texture(sheet, {0, 60, 120, 60}, {20, 0, 80, 60});

    return "";
}

std::string Textures::loadAllTextures() {
    auto msg = loadResourceList<sf::Texture>({{"tile", "media/tile.png"},
                                              {"menu_light", "media/menubackground_light.png"},
                                              {"menu_dark", "media/menubackground_dark.png"},
                                              {"background_light", "media/background_light.png"},
                                              {"background_dark", "media/background_dark.png"},
                                              {"alert", "media/alert.png"},
                                              {"rotate", "media/rotate.png"},
                                              {"choosecolor", "media/choosecolor.png"},
                                              {"logo", "media/blackwhitelogo.png"},
                                              {"icon", "media/icon.png"},
                                              {"uisheet", "media/neutralUIsheet.png"}});
    if (msg != "") return msg;

    // Set default background for panels

    os::Picture::default_texture = tgui::Texture(texture("uisheet"), {240, 0, 250, 250}, {30, 30, 190, 190});
    os::Picture::default_texture.setSmooth(true);

    // Load tileSet

    auto& map = getMap<sf::Texture>();
    auto it = map.find("tile");
    if (it == map.end()) return "No tile texture for TileSet";

    tiles[0].setTexture(it->second);
    tiles[0].setColor(sf::Color(255, 0, 0));
    tiles[1].setTexture(it->second);
    tiles[1].setColor(sf::Color(0, 255, 0));
    tiles[2].setTexture(it->second);
    tiles[2].setColor(sf::Color(115, 145, 255));
    tiles[3].setTexture(it->second);
    tiles[3].setColor(sf::Color(255, 0, 255));
    tiles[4].setTexture(it->second);
    tiles[4].setColor(sf::Color(0, 255, 255));
    tiles[5].setTexture(it->second);
    tiles[5].setColor(sf::Color(255, 255, 0));
    tiles[6].setTexture(it->second);
    tiles[6].setColor(sf::Color(255, 255, 255));
    tiles[7].setTexture(it->second);
    tiles[7].setColor(sf::Color(170, 170, 170));

    tiles[8].setTexture(it->second);
    tiles[8].setColor(sf::Color(255, 0, 0, 120));
    tiles[9].setTexture(it->second);
    tiles[9].setColor(sf::Color(0, 255, 0, 120));
    tiles[10].setTexture(it->second);
    tiles[10].setColor(sf::Color(115, 145, 255, 120));
    tiles[11].setTexture(it->second);
    tiles[11].setColor(sf::Color(255, 0, 255, 120));
    tiles[12].setTexture(it->second);
    tiles[12].setColor(sf::Color(0, 255, 255, 120));
    tiles[13].setTexture(it->second);
    tiles[13].setColor(sf::Color(255, 255, 0, 120));
    tiles[14].setTexture(it->second);
    tiles[14].setColor(sf::Color(255, 255, 255, 120));
    tiles[15].setTexture(it->second);
    tiles[15].setColor(sf::Color(170, 170, 170, 120));

    if ((msg = loadThemeResources()) != "") return msg;

    for (auto& texture : map) texture.second.setSmooth(true);

    return "";
}

std::string loadAllFonts() {
    auto msg =
        loadResourceList<sf::Font>({{"typewriter", "media/Kingthings Trypewriter 2.ttf"}, {"print", "media/F25_Bank_Printer.ttf"}, {"standard", "media/standard.ttf"}});
    if (msg != "") return msg;

    return "";
}

sf::Sprite Textures::sprite(const std::string& name) { return sf::Sprite(getMap<sf::Texture>()[name]); }

std::array<sf::Sprite, 16>& Textures::tileSet() { return tiles; }

sf::Font& Textures::font(const std::string& name) { return getMap<sf::Font>()[name]; }

sf::Texture& Textures::texture(const std::string& name) { return getMap<sf::Texture>()[name]; }

sf::Color& Textures::color(const std::string& name) { return getMap<sf::Color>()[name]; }

tgui::Texture& Textures::guiTexture(const std::string& name) { return getMap<tgui::Texture>()[name]; }

std::string Textures::loadTextures(sf::RenderWindow& window) {
    std::string msg;
    if ((msg = loadAllTextures()) != "") return msg;

    if ((msg = loadAllFonts()) != "") return msg;

    theme1.load(resourcePath() + "neutralUI.txt");
    tgui::Theme::setDefault(&theme1);
    try {
        tgui::Button::Ptr test = tgui::Button::create();
    } catch (tgui::Exception) {
        return "neutralUI.txt";
    }

    tGui.setTarget(window);
    tGui.setFont(font("standard"));

    color("GameFieldText") = sf::Color(0, 0, 0, 255);

    return "OK";
}

Textures::Textures(sf::RenderWindow& window) : tGui(window) {}

void Textures::setGhostPieceAlpha(uint8_t alpha) {
    tiles[8].setColor(sf::Color(255, 0, 0, alpha));
    tiles[9].setColor(sf::Color(0, 255, 0, alpha));
    tiles[10].setColor(sf::Color(115, 145, 255, alpha));
    tiles[11].setColor(sf::Color(255, 0, 255, alpha));
    tiles[12].setColor(sf::Color(0, 255, 255, alpha));
    tiles[13].setColor(sf::Color(255, 255, 0, alpha));
    tiles[14].setColor(sf::Color(255, 255, 255, alpha));
    tiles[15].setColor(sf::Color(170, 170, 170, alpha));
}
