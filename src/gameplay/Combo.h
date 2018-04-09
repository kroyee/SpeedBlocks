#ifndef COMBO_H
#define COMBO_H

#include <SFML/Graphics.hpp>
#include <mutex>

class ComboCounter {
private:
	sf::Time comboStart=sf::seconds(0), comboTime=sf::seconds(0);
	uint8_t lineCount=0;
	std::mutex comboMutex;
public:
	ComboCounter(uint16_t& _maxCombo) : maxCombo(_maxCombo) {}
	
	uint16_t & maxCombo;
	uint8_t comboCount=0;

	void clear();
	void increase(const sf::Time& t, uint8_t amount);
	uint16_t check(const sf::Time& t);
	void noClear();
	uint8_t timerCount(const sf::Time& t);
};

#endif