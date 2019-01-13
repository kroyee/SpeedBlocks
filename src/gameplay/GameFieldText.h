#ifndef GAMEFIELDTEXT_H
#define GAMEFIELDTEXT_H

#include <SFML/Graphics.hpp>
#include <mutex>

enum class FieldText {
	Name, Away, Position, Ready, Combo, Pending, Countdown, BPM, GameOver,

	COUNT
};

class TextObject {
public:
	TextObject();

	TextObject& set(const std::string& str);
	TextObject& set(int32_t val);

	TextObject& header(const std::string& str, uint16_t offset = 5);

	TextObject& size(uint8_t text_size, uint8_t header_size = 0);

	TextObject& setFont(const sf::Font& font);

	TextObject& position(int x, int y);

	TextObject& color(const sf::Color& color);

	TextObject& hide();

	TextObject& show();

	void draw(sf::RenderTexture& texture);

	int getTextWidth();

	int32_t get();

private:
	void update_origin();

	sf::Text headerText, text;
	bool useHeader=false, visible=true;
	int32_t value=0;
	uint16_t header_offset=5;
};

class Resources;

class GameFieldText {
    Resources& resources;

	std::vector<TextObject> text;

    std::mutex fieldTextMutex;

	sf::RenderTexture& texture;
public:
	sf::ConvexShape comboTimer;
    std::string name;

    GameFieldText(Resources& _resources, sf::RenderTexture& tex);

	template <FieldText FT, typename T>
	void set(const T& t) {
		std::lock_guard<std::mutex> mute(fieldTextMutex);
		text[static_cast<unsigned>(FT)].set(t);
	}

	template <FieldText FT>
	void hide() {
		std::lock_guard<std::mutex> mute(fieldTextMutex);
		text[static_cast<unsigned>(FT)].hide();
	}

	template <FieldText FT>
	void show() {
		std::lock_guard<std::mutex> mute(fieldTextMutex);
		text[static_cast<unsigned>(FT)].show();
	}

	template <FieldText FT>
	int get() {
		std::lock_guard<std::mutex> mute(fieldTextMutex);
		return text[static_cast<unsigned>(FT)].get();
	}

    void setName(const std::string& n);
    void setPosition(int8_t _position);
    void setColor(sf::Color color);
	void setFont(const sf::Font& font);
	bool setComboTimer(uint8_t count);

    void clear();

    void drawText();
};

#endif
