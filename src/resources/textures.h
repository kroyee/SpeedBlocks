#ifndef TEXTURES_H
#define TEXTURES_H

#include <TGUI/TGUI.hpp>
#include <array>

class textures {
public:
    textures(sf::RenderWindow& window);
	sf::Texture tileTexture;
	std::array<sf::Sprite, 16> tile;

    sf::Texture logoTexture;
    sf::Sprite logo;

    sf::Texture background_light;
    sf::Texture background_dark;

    sf::Texture menuBackground_light;
    sf::Texture menuBackground_dark;

    sf::Texture alert;

    sf::Image* icon;

    sf::Font typewriter, printFont, standard;

    sf::Texture rotate, color;

    tgui::Theme::Ptr themeTG;

    tgui::Gui tGui;

    sf::Color gameFieldTextColor;

    std::string loadTextures();
    void setGhostPieceAlpha(uint8_t alpha);
};

#endif
