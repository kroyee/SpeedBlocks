#include "AnimatedBackground.h"
#include <iostream>
using std::cout;
using std::endl;

const sf::Color color(128, 128, 128);
const float speedfactor = 0.20;

MovingPoint::MovingPoint(sf::Vector2f start) {
	start.x -= (int)start.x % 30;
	start.y -= (int)start.y % 30;
	for (int i=0; i<5; i++)
		pos.push_back(start);
	turnVal = 1;
	direction = rand() % 4;
	distance = 0;
}

void MovingPoint::drawFirst(sf::RenderWindow& window) {
	sf::RectangleShape line;
	line.setFillColor(color);
	line.setPosition(pos.front());
	line.setOutlineThickness(1);
	line.setOutlineColor(color);
	if (direction == 0)
		line.setSize({distance, 1});
	else if (direction == 1)
		line.setSize({1, distance});
	else if (direction == 2)
		line.setSize({-distance, 1});
	else
		line.setSize({1, -distance});
	window.draw(line);
}

void MovingPoint::drawLines(sf::RenderWindow& window) {
	sf::RectangleShape line;
	line.setFillColor(color);
	auto it1 = pos.begin();
	auto it2 = it1;
	it2++;
	while (it2 != --pos.end()) {
		line.setPosition(*it1);
		sf::Vector2f size = *it2 - *it1;
		if (size.x == 0)
			size.x=1;
		if (size.y == 0)
			size.y=1;
		line.setOutlineThickness(1);
		line.setOutlineColor(color);
		line.setSize(size);
		window.draw(line);
		it1++;
		it2++;
	}
}

void MovingPoint::drawLast(sf::RenderWindow& window) {
	sf::RectangleShape line;
	line.setFillColor(color);
	auto it1 = pos.end();
	it1--;
	auto it2 = it1;
	it1--;

	line.setPosition(*it1);
	sf::Vector2f size = *it2 - *it1;
	if (size.x == 0)
		size.x = 1;
	else if (size.x < 0)
		size.x = distance-30;
	else
		size.x = 30-distance;
	if (size.y == 0)
		size.y = 1;
	else if (size.y < 0)
		size.y = distance-30;
	else
		size.y = 30-distance;
	line.setSize(size);
	line.setOutlineThickness(1);
	line.setOutlineColor(color);
	window.draw(line);
}

AnimatedBackground::AnimatedBackground(sf::Uint8 count) {
	for (int i=0; i<count; i++)
		points.push_back(MovingPoint(sf::Vector2f(i*960/count, i*600/count)));
}

void AnimatedBackground::draw(sf::RenderWindow& window, const sf::Time& current) {
	window.clear(sf::Color(70, 70, 70));
	update(current);
	for (auto& point : points) {
		point.drawFirst(window);
		point.drawLines(window);
		point.drawLast(window);
	}
}

void AnimatedBackground::update(const sf::Time& current) {
	float delta = (current-lastFrame).asMilliseconds() * speedfactor;
	lastFrame=current;
	for (auto& point : points) {
		point.distance += delta;
		while (point.distance > 30) {
			point.pos.push_front(point.pos.front());
			if (point.direction == 0)
				point.pos.front().x +=30;
			else if (point.direction == 1)
				point.pos.front().y +=30;
			else if (point.direction == 2)
				point.pos.front().x -=30;
			else
				point.pos.front().y -=30;
			point.distance-=30;
			sf::Uint8 turn = rand() % 3;
			if (turn == 2) {}
			else if (turn < point.turnVal) {
				point.turnVal--;
				point.direction--;
				if (point.direction > 3)
					point.direction=3;
			}
			else {
				point.turnVal++;
				point.direction++;
				if (point.direction > 3)
					point.direction=0;
			}

			if (point.pos.front().x == 1050) {
				point.direction = 2;
				point.turnVal=1;
			}
			else if (point.pos.front().x == -90) {
				point.direction = 0;
				point.turnVal=1;
			}
			else if (point.pos.front().y == 690) {
				point.direction = 3;
				point.turnVal=1;
			}
			else if (point.pos.front().y == -90) {
				point.direction = 1;
				point.turnVal=1;
			}

			point.pos.pop_back();
		}
	}
}