#ifndef COMBO_H
#define COMBO_H

#include <SFML/Graphics.hpp>

class ComboCounter {
private:
	sf::Time comboStart=sf::seconds(0), comboTime=sf::seconds(0);
	sf::Uint8 lineCount=0;
public:
	ComboCounter(uint16_t& _maxCombo) : maxCombo(_maxCombo) {}
	
	uint16_t & maxCombo;
	sf::Uint8 comboCount=0;

	void clear();
	void increase(const sf::Time& t, sf::Uint8 amount);
	sf::Uint16 check(const sf::Time& t);
	void noClear();
	sf::Uint8 timerCount(const sf::Time& t);
};

#endif