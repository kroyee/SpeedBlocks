#ifndef TEXTURES_H
#define TEXTURES_H

#define SFML_NO_DEPRECATED_WARNINGS
#include <SFML/Graphics.hpp>

class textures {
public:
	sf::Texture tileTexture;
	sf::Sprite tile[16];

	sf::Texture fieldBackgroundTexture;
    sf::Sprite fieldBackground;

    sf::Texture backgroundTexture;
    sf::Sprite background;

    sf::String loadTextures();
};

#endif