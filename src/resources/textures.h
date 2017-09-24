#ifndef TEXTURES_H
#define TEXTURES_H

#include <TGUI/TGUI.hpp>

class textures {
public:
    textures(sf::RenderWindow& window);
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

    tgui::Theme::Ptr themeTG;

    tgui::Gui tGui;

    sf::Color gameFieldTextColor;

    sf::String loadTextures();
    void setGhostPieceAlpha(sf::Uint8 alpha);
};

#endif
