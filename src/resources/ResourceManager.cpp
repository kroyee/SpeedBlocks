#include "ResourceManager.h"
#include <map>
#include <iostream>
#include <utility>

#ifdef __APPLE__
#include "ResourcePath.hpp"
#else
#include "EmptyResourcePath.h"
#endif

namespace Resources {
	static std::map<std::string, sf::Texture> textureMap;
	static std::array<sf::Sprite, 16> tileSet;
	static sf::Image icon;
	static sf::Color gameFieldTextColor;

	tgui::Theme::Ptr themeTG;
	tgui::Gui tGui;

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

	std::string loadAllTextures() {
		auto msg = loadResourceList<sf::Texture>({
			{ "tile", "media/tile.png" },
			{ "menu_light", "media/menubackground_light.png" },
			{ "menu_dark", "media/menubackground_dark.png" },
			{ "background_light", "media/background_light.png" },
			{ "background_dark", "media/background_dark.png" },
			{ "alert", "media/alert.png" },
			{ "rotate", "media/rotate.png" },
			{ "choosecolor", "media/choosecolor.png" },
			{ "logo", "media/blackwhitelogo.png" }
		});
		if (msg != "")
			return msg;

		if (!icon.loadFromFile(resourcePath() + "media/icon.png"))
			return "media/icon.png";

		// Load tileSet

		auto& map = getMap<sf::Texture>();
		auto it = map.find("tile");
		if (it == textureMap.end())
			return "No tile texture for TileSet";

		tileSet[0].setTexture(it->second); tileSet[0].setColor(sf::Color(255,0,0));
	    tileSet[1].setTexture(it->second); tileSet[1].setColor(sf::Color(0,255,0));
	    tileSet[2].setTexture(it->second); tileSet[2].setColor(sf::Color(115,145,255));
	    tileSet[3].setTexture(it->second); tileSet[3].setColor(sf::Color(255,0,255));
	    tileSet[4].setTexture(it->second); tileSet[4].setColor(sf::Color(0,255,255));
	    tileSet[5].setTexture(it->second); tileSet[5].setColor(sf::Color(255,255,0));
	    tileSet[6].setTexture(it->second); tileSet[6].setColor(sf::Color(255,255,255));
	    tileSet[7].setTexture(it->second); tileSet[7].setColor(sf::Color(170,170,170));

	    tileSet[8].setTexture(it->second); tileSet[8].setColor(sf::Color(255,0,0,120));
	    tileSet[9].setTexture(it->second); tileSet[9].setColor(sf::Color(0,255,0,120));
	    tileSet[10].setTexture(it->second); tileSet[10].setColor(sf::Color(115,145,255,120));
	    tileSet[11].setTexture(it->second); tileSet[11].setColor(sf::Color(255,0,255,120));
	    tileSet[12].setTexture(it->second); tileSet[12].setColor(sf::Color(0,255,255,120));
	    tileSet[13].setTexture(it->second); tileSet[13].setColor(sf::Color(255,255,0,120));
	    tileSet[14].setTexture(it->second); tileSet[14].setColor(sf::Color(255,255,255,120));
	    tileSet[15].setTexture(it->second); tileSet[15].setColor(sf::Color(170,170,170,120));

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

	sf::Sprite getSprite(std::string name) {
		auto& map = getMap<sf::Texture>();
		auto it = map.find(name);
		if (it == map.end()) {
			std::cout << "Sprite does not exist" << std::endl;
			return {};
		}

		return sf::Sprite(it->second);
	}

	std::array<sf::Sprite, 16>& getTileSet() {
		return tileSet;
	}

	sf::Font getFont(std::string name) {
		return getMap<sf::Font>()[name];
	}

	sf::Texture& getTexture(std::string name) {
		return getMap<sf::Texture>()[name];
	}

	void setGhostPieceAlpha(uint8_t alpha) {
	    tileSet[8].setColor(sf::Color(255,0,0,alpha));
	    tileSet[9].setColor(sf::Color(0,255,0,alpha));
	    tileSet[10].setColor(sf::Color(115,145,255,alpha));
	    tileSet[11].setColor(sf::Color(255,0,255,alpha));
	    tileSet[12].setColor(sf::Color(0,255,255,alpha));
	    tileSet[13].setColor(sf::Color(255,255,0,alpha));
	    tileSet[14].setColor(sf::Color(255,255,255,alpha));
	    tileSet[15].setColor(sf::Color(170,170,170,alpha));
	}

	void setFieldTextColor(sf::Color& color) {
		gameFieldTextColor = color;
	}

	sf::Color& getFieldTextColor() {
		return gameFieldTextColor;
	}

	sf::Image& getIcon() {
		return icon;
	}

	std::string loadAllResources(sf::RenderWindow& window) {
		std::string msg;
		if ((msg = loadAllTextures()) != "")
			return msg;

		if ((msg = loadAllFonts()) != "")
			return msg;

		themeTG = tgui::Theme::create(resourcePath() + "media/TransparentGrey.txt");
	    tgui::Label::Ptr test;
	    try {
	        test = themeTG->load("Label");
	    } catch (tgui::Exception) {
	        return "media/TransparentGrey.txt";
	    }

	    tGui.setWindow(window);
	    tGui.setFont(getFont("standard"));

	    return "";
	}
}