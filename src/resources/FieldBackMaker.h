#ifndef FIELDBACKMAKER_H
#define FIELDBACKMAKER_H

#include <SFML/Graphics.hpp>

// Style 1 = full lines
// Style 2 = intersection crosses
// Style 3 = no transparency at intersection, fading in between

void makeVerticalLines(sf::RenderTexture* texture, uint8_t style);

void makeHorizontalLines(sf::RenderTexture* texture, uint8_t style);

sf::Texture makeBackground(bool vertical, bool horizontal, uint8_t style, bool lineColorVal);

#endif