#ifndef TEXTURES_H
#define TEXTURES_H

#include <TGUI/TGUI.hpp>
#include <array>

class Textures {
   public:
    using TileSetType = std::array<sf::Sprite, 16>;
    Textures(sf::RenderWindow& window);

    std::string loadTextures(sf::RenderWindow& window);
    std::string loadAllTextures();
    void setGhostPieceAlpha(uint8_t alpha);

    sf::Sprite sprite(const std::string& name);
    TileSetType& tileSet();
    sf::Font& font(const std::string& name);
    sf::Texture& texture(const std::string& name);
    sf::Color& color(const std::string& name);
    tgui::Texture& guiTexture(const std::string& name);

    tgui::Gui tGui;
    tgui::Theme theme1;

   private:
    TileSetType tiles;
};

#endif
