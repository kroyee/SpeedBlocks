#ifndef TEXTURES_H
#define TEXTURES_H

#include <SFML/Graphics.hpp>
#include <TGUI/TGUI.hpp>

class textures {
public:
	sf::Texture tileTexture;
	sf::Sprite tile[16];

    sf::Texture logoTexture;
    sf::Sprite logo;

    sf::Texture background_light;
    sf::Texture background_dark;

    sf::Texture menuBackground_light;
    sf::Texture menuBackground_dark;

    sf::Texture alert;

    sf::Image* icon;

    sf::Font typewriter, printFont, standard;

    tgui::Texture rotate_n, rotate_h, color_n, color_h;

    sf::Color gameFieldTextColor;

    sf::String loadTextures();
    void setGhostPieceAlpha(sf::Uint8 alpha);
};

#endif
