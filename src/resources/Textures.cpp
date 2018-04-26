#include "Textures.h"
#include <TGUI/TGUI.hpp>
#include <iostream>

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
  if (map.find(name) != map.end())
    return {"Resource " + name + " already loaded"};

  if (!map[name].loadFromFile(resourcePath() + filename))
    return filename;

  return "";
}

template <typename T>
std::string loadResourceList(std::vector<std::pair<std::string, std::string>> names) {
  for (auto& name : names) {
    auto msg = loadResource<T>(name.first, name.second);
    if (msg != "")
      return msg;
  }

  return "";
}

std::string Textures::loadAllTextures() {
  auto msg = loadResourceList<sf::Texture>({
    { "tile", "media/tile.png" },
    { "menu_light", "media/menubackground_light.png" },
    { "menu_dark", "media/menubackground_dark.png" },
    { "background_light", "media/background_light.png" },
    { "background_dark", "media/background_dark.png" },
    { "alert", "media/alert.png" },
    { "rotate", "media/rotate.png" },
    { "choosecolor", "media/choosecolor.png" },
    { "logo", "media/blackwhitelogo.png" },
    { "icon", "media/icon.png" }
  });
  if (msg != "")
    return msg;

  // Load tileSet

  auto& map = getMap<sf::Texture>();
  auto it = map.find("tile");
  if (it == map.end())
    return "No tile texture for TileSet";

  tiles[0].setTexture(it->second); tiles[0].setColor(sf::Color(255,0,0));
  tiles[1].setTexture(it->second); tiles[1].setColor(sf::Color(0,255,0));
  tiles[2].setTexture(it->second); tiles[2].setColor(sf::Color(115,145,255));
  tiles[3].setTexture(it->second); tiles[3].setColor(sf::Color(255,0,255));
  tiles[4].setTexture(it->second); tiles[4].setColor(sf::Color(0,255,255));
  tiles[5].setTexture(it->second); tiles[5].setColor(sf::Color(255,255,0));
  tiles[6].setTexture(it->second); tiles[6].setColor(sf::Color(255,255,255));
  tiles[7].setTexture(it->second); tiles[7].setColor(sf::Color(170,170,170));

  tiles[8].setTexture(it->second); tiles[8].setColor(sf::Color(255,0,0,120));
  tiles[9].setTexture(it->second); tiles[9].setColor(sf::Color(0,255,0,120));
  tiles[10].setTexture(it->second); tiles[10].setColor(sf::Color(115,145,255,120));
  tiles[11].setTexture(it->second); tiles[11].setColor(sf::Color(255,0,255,120));
  tiles[12].setTexture(it->second); tiles[12].setColor(sf::Color(0,255,255,120));
  tiles[13].setTexture(it->second); tiles[13].setColor(sf::Color(255,255,0,120));
  tiles[14].setTexture(it->second); tiles[14].setColor(sf::Color(255,255,255,120));
  tiles[15].setTexture(it->second); tiles[15].setColor(sf::Color(170,170,170,120));

  for (auto& texture : map)
    texture.second.setSmooth(true);

  return "";
}

std::string loadAllFonts() {
  auto msg = loadResourceList<sf::Font>({
    { "typewriter", "media/Kingthings Trypewriter 2.ttf" },
    { "print", "media/F25_Bank_Printer.ttf" },
    { "standard", "media/standard.ttf" }
  });
  if (msg != "")
    return msg;

  return "";
}

sf::Sprite Textures::sprite(const std::string& name) {
  return sf::Sprite(getMap<sf::Texture>()[name]);
}

std::array<sf::Sprite, 16>& Textures::tileSet() {
  return tiles;
}

sf::Font& Textures::font(const std::string& name) {
  return getMap<sf::Font>()[name];
}

sf::Texture& Textures::texture(const std::string& name) {
  return getMap<sf::Texture>()[name];
}

sf::Color& Textures::color(const std::string& name) {
  return getMap<sf::Color>()[name];
}

tgui::Texture& Textures::guiTexture(const std::string& name) {
  return getMap<tgui::Texture>()[name];
}

std::string Textures::loadTextures(sf::RenderWindow& window) {
  std::string msg;
  if ((msg = loadAllTextures()) != "")
    return msg;

  if ((msg = loadAllFonts()) != "")
    return msg;

  theme1 = tgui::Theme::create(resourcePath() + "neutralUI.txt");
  tgui::Label::Ptr test;
  try {
      test = theme1->load("Label");
  } catch (tgui::Exception) {
      return "media/TransparentGrey.txt";
  }

  tGui.setWindow(window);
  tGui.setFont(font("standard"));

  color("GameFieldText") = sf::Color(0,0,0,255);

  return "OK";
}

Textures::Textures(sf::RenderWindow& window) : tGui(window) {}

/*std::string textures::loadTextures() {
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

    themeTG = tgui::Theme::create(resourcePath() + "neutralUI.txt");
    tgui::Label::Ptr test;
    try {
        test = themeTG->load("Label");
    } catch (tgui::Exception) {
        return "media/TransparentGrey.txt";
    }

    tGui.setFont(standard);

    return "OK";
}*/

void Textures::setGhostPieceAlpha(uint8_t alpha) {
    tiles[8].setColor(sf::Color(255,0,0,alpha));
    tiles[9].setColor(sf::Color(0,255,0,alpha));
    tiles[10].setColor(sf::Color(115,145,255,alpha));
    tiles[11].setColor(sf::Color(255,0,255,alpha));
    tiles[12].setColor(sf::Color(0,255,255,alpha));
    tiles[13].setColor(sf::Color(255,255,0,alpha));
    tiles[14].setColor(sf::Color(255,255,255,alpha));
    tiles[15].setColor(sf::Color(170,170,170,alpha));
}

tgui::WidgetConverter Textures::load(const std::string& widget) {
	return theme1->load(widget);
}

tgui::WidgetConverter Textures::load(const std::string& widget, const sf::Vector2f& pos) {
	auto wcnv = load(widget);
	tgui::Widget::Ptr wptr = wcnv;
	wptr->setSize(pos.x, pos.y);
	return wcnv;
}

tgui::WidgetConverter Textures::load(const std::string& widget, const std::string& text) {
	auto wcnv = load(widget);
	tgui::Widget::Ptr wptr = wcnv;

	if (wptr->getWidgetType() == "Button")
		static_cast<tgui::Button*>(wptr.get())->setText(text);
	else if (wptr->getWidgetType() == "Label")
		static_cast<tgui::Label*>(wptr.get())->setText(text);

	return wcnv;
}

tgui::WidgetConverter Textures::load(const std::string& widget, const std::string& text, const sf::Vector2f& pos) {
	auto wcnv = load(widget, text);
	tgui::Widget::Ptr wptr = wcnv;
	wptr->setSize(pos.x, pos.y);
	return wcnv;
}
