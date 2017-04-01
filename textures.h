#ifndef TEXTURES_H
#define TEXTURES_H

#include <SFML/Graphics.hpp>

class textures {
public:
	sf::Texture tileTexture;
	sf::Sprite tile[18];

	sf::Texture fieldBackgroundTexture;
    sf::Sprite fieldBackground;

    sf::Texture backgroundTexture;
    sf::Sprite background;

    sf::String loadTextures();
};

#endif
