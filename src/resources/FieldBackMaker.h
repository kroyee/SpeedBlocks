#ifndef FIELDBACKMAKER_H
#define FIELDBACKMAKER_H

#include <SFML/Graphics.hpp>

// Style 1 = full lines
// Style 2 = intersection crosses
// Style 3 = no transparency at intersection, fading in between

void makeVerticalLines(sf::RenderTexture* texture, sf::Uint8 style);

void makeHorizontalLines(sf::RenderTexture* texture, sf::Uint8 style);

sf::Texture makeBackground(bool vertical, bool horizontal, sf::Uint8 style, bool lineColorVal);

#endif