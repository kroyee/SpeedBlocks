#ifndef GAMEFIELDTEXT_H
#define GAMEFIELDTEXT_H

#include <SFML/Graphics.hpp>
#include <mutex>

class Resources;

class GameFieldText {
    Resources& resources;

	sf::Text nameTag;
    sf::Text awayText;
    sf::Text positionText;
    sf::Text readyText;
    sf::Text comboText;
    sf::Text pendingText;
    sf::Text countdownText;
    sf::Text bpmText;
    sf::Text gameOverText;

    std::mutex fieldTextMutex;
public:
    sf::RenderTexture* texture;
    
    sf::ConvexShape comboTimer;

    uint8_t combo, pending;
    uint16_t bpm;
    int8_t position, countdown, gameover;
    bool away, ready;

    sf::String name;

    GameFieldText(Resources& _resources);
    GameFieldText(const GameFieldText& text);

    void setName(const sf::String& n);
    void setPosition(const int8_t _position);
    void setCountdown(const int8_t _countdown);
    void setGameover(const int8_t _gameover);
    bool setComboTimer(uint8_t count);
    void setBpm(const uint16_t _bpm);
    void setCombo(const uint8_t _combo);
    void setPending(const uint8_t _pending);
    void setColor(sf::Color color);

    void clear();

    void drawText();
};

#endif