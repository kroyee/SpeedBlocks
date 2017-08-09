#ifndef ANIMATEDBACKGROUND_H
#define ANIMATEDBACKGROUND_H

#include <SFML/Graphics.hpp>
#include <list>

class Resources;

struct MovingPoint {
	MovingPoint(sf::Vector2f start);
	std::list<sf::Vector2f> pos;
	sf::Uint8 turnVal, direction;
	float distance;
	sf::Color color;

	void drawFirst(sf::RenderWindow& window);
	void drawLines(sf::RenderWindow& window);
	void drawLast(sf::RenderWindow& window);
};

class AnimatedBackground {
private:
	std::vector<MovingPoint> points;

	sf::Time lastFrame;
	sf::Texture &background_light, &background_dark;
	sf::Sprite backSprite;
	bool enabled;
	sf::Color color;
public:
	AnimatedBackground(Resources& resources, sf::Uint8 count);
	void draw(sf::RenderWindow& window, const sf::Time& current);
	void update(const sf::Time& current);
	void enable(const sf::Time& current);
	void disable();
	void dark();
	void light();
};

#endif