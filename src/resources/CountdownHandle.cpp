#include "CountdownHandle.h"
#include <iostream>

void CountdownHandle::set(const sf::Time& t, uint16_t countdown) { countdownTime = std::min(countdownTime, t - (sf::milliseconds(3000) - sf::milliseconds(countdown))); }

uint8_t CountdownHandle::check(const sf::Time& t) {
    uint8_t currentcd = 4 - (t - countdownTime).asSeconds();
    if (currentcd < countdownCount) {
        countdownCount = currentcd;
        if (!currentcd) countdownOngoing = false;
        return countdownCount;
    }
    return 255;
}

void CountdownHandle::start(const sf::Time& t) {
    countdownOngoing = true;
    countdownCount = 3;
    countdownTime = t;
}

void CountdownHandle::stop() { countdownOngoing = false; }

bool CountdownHandle::ongoing() { return countdownOngoing; }