#include "Combo.h"
#include <cmath>

void ComboCounter::clear() {
	comboStart=sf::seconds(0);
	comboTime=sf::seconds(0);
	maxCombo=0;
	comboCount=0;
}

void ComboCounter::increase(const sf::Time& t, sf::Uint8 amount) {
	if (comboCount==0) {
		comboStart=t;
		comboTime=sf::seconds(0);
	}
	comboCount++;
	comboTime+=sf::seconds((0.8/comboCount) + ((amount)/2.0)*(1.1/comboCount));

	if (comboCount>maxCombo)
		maxCombo=comboCount;
}

sf::Uint16 ComboCounter::check(const sf::Time& t) {
	if (t > comboStart+comboTime && comboCount!=0) {
		float durationMultiplyer = 1 + (float)t.asSeconds() / 60.0 * 0.1;
		sf::Uint16 comboLinesSent = comboCount * pow(1.125, comboCount) * durationMultiplyer;
		comboCount = 0;

		return comboLinesSent;
	}
	return 0;
}

void ComboCounter::noClear() {
	comboTime-=sf::milliseconds(200);
}

sf::Uint8 ComboCounter::timerCount(const sf::Time& t) {
	sf::Time timeleft = comboStart + comboTime - t;
	short count = (timeleft.asMilliseconds()/6.0) / 10.0;
	if (count>100)
		count=100;

	if (count<0)
		count=0;

	return count;
}