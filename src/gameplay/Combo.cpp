#include "Combo.h"
#include <cmath>

const float lineMultiplyer = 1.0;
const float staticMultiplyer = 0.8;

void ComboCounter::clear() {
	std::lock_guard<std::mutex> mute(comboMutex);
	comboStart=sf::seconds(0);
	comboTime=sf::seconds(0);
	maxCombo=0;
	comboCount=0;
}

void ComboCounter::increase(const sf::Time& t, sf::Uint8 amount) {
	std::lock_guard<std::mutex> mute(comboMutex);
	if (comboCount==0) {
		comboStart=t;
		comboTime=sf::seconds(0);
	}
	comboCount++;
	float lineTime = 0;
	for (int i=0; i<amount; i++) {
		lineCount++;
		lineTime+=lineMultiplyer/lineCount;
	}
	comboTime+=sf::seconds(staticMultiplyer/comboCount + lineTime);

	if (comboCount>maxCombo)
		maxCombo=comboCount;
}

sf::Uint16 ComboCounter::check(const sf::Time& t) {
	std::lock_guard<std::mutex> mute(comboMutex);
	if (t > comboStart+comboTime && comboCount!=0) {
		float durationMultiplyer = 1 + (float)t.asSeconds() / 60.0 * 0.1;
		sf::Uint16 comboLinesSent = pow(comboCount, 1.4+comboCount*0.01) * durationMultiplyer;
		comboCount = 0;
		lineCount = 0;

		return comboLinesSent;
	}
	return 0;
}

void ComboCounter::noClear() {
	std::lock_guard<std::mutex> mute(comboMutex);
	comboTime-=sf::milliseconds(200);
}

sf::Uint8 ComboCounter::timerCount(const sf::Time& t) {
	std::lock_guard<std::mutex> mute(comboMutex);
	sf::Time timeleft = comboStart + comboTime - t;
	short count = (timeleft.asMilliseconds()/6.0) / 10.0;
	if (count>100)
		count=100;

	if (count<0)
		count=0;

	return count;
}