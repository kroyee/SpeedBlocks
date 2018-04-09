#include "FieldBackMaker.h"
#include <iostream>
using std::cout;
using std::endl;

void makeHorizontalLines(uint8_t pixels[], sf::Color color, uint8_t style) {
	uint8_t a;
	for (int y=29; y<511; y++) {
		int check = y%30;
		if (check == 29 || check == 0)
			for (int x=0; x<300; x++) {
				if (style == 1)
					a=color.a;
				else if (style == 2) {
					a=0;
					if (x > 15 && x < 285)
						if (x % 30 < 5 || x % 30 > 24)
							a=color.a;
				}
				else if (style == 3) {
					int8_t tmp = x%30;
					if (tmp < 15)
						a=color.a*(1-tmp/20.0);
					else
						a=color.a*((tmp-9)/20.0);
				}
				uint32_t pix = (x + y*300)*4;
				pixels[pix] = color.r;
				pixels[pix+1] = color.g;
				pixels[pix+2] = color.b;
				pixels[pix+3] = a;
			}
	}
}

void makeVerticalLines(uint8_t pixels[], sf::Color color, uint8_t style) {
	uint8_t a;
	for (int x=29; x<271; x++) {
		int check = x%30;
		if (check == 29 || check == 0)
			for (int y=0; y<540; y++) {
				if (style == 1)
					a=color.a;
				else if (style == 2) {
					a=0;
					if (y > 15 && y < 525)
						if (y % 30 < 4 || y % 30 > 25)
							a=color.a;
				}
				else if (style == 3){
					int8_t tmp = y%30;
					if (tmp < 15)
						a=color.a*(1-tmp/20.0);
					else
						a=color.a*((tmp-9)/20.0);
				}
				uint32_t pix = (x + y*300)*4;
				pixels[pix] = color.r;
				pixels[pix+1] = color.g;
				pixels[pix+2] = color.b;
				pixels[pix+3] = a;
			}
	}
}

sf::Texture makeBackground(bool vertical, bool horizontal, uint8_t style, bool lineColorVal) {
	sf::RenderTexture texture;
	texture.create(300, 540);
	texture.clear(sf::Color(0,0,0,0));

	sf::Color lineColor;
	if (lineColorVal)
		lineColor = sf::Color(0,0,0,200);
	else
		lineColor = sf::Color(255,255,255,200);

	uint8_t pixels[540*300*4] = { };
	if (vertical)
		makeVerticalLines(pixels, lineColor, style);
	if (horizontal)
		makeHorizontalLines(pixels, lineColor, style);

	sf::Image back;
	back.create(300, 540, pixels);

	sf::Texture backtex;
	backtex.loadFromImage(back);

	sf::Sprite backSprite;
	backSprite.setTexture(backtex);

	texture.draw(backSprite);

	texture.display();

	return texture.getTexture();
}