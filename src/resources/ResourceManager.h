#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#include <TGUI/TGUI.hpp>
#include <string>

namespace Resources {

	///////////////////
	// Get resources //
	///////////////////

	sf::Sprite getSprite(std::string name);

	std::array<sf::Sprite, 16>& getTileSet();

	sf::Font getFont(std::string name);

	sf::Texture& getTexture(std::string name);

	sf::Image& getIcon();


	////////////////////////////////////////////
	// Getters and setters for certain values //
	////////////////////////////////////////////

	void setGhostPieceAlpha(uint8_t alpha);

	void setFieldTextColor(sf::Color& color);

	sf::Color& getFieldTextColor();


	////////////////////
	// Initialization //
	////////////////////

	std::string loadAllResources(sf::RenderWindow& window);


	///////////////////
	// Direct access //
	///////////////////

	extern tgui::Theme::Ptr themeTG;

	extern tgui::Gui tGui;
}

#endif