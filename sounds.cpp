#include "sounds.h"
#include <iostream>
using namespace std;

soundBank::soundBank() {
	garbAddBuff.loadFromFile("sounds/173958__fins__failure.wav");
	lineBlockBuff.loadFromFile("sounds/350905__cabled-mess__jump-c-05.wav");
	menuSelBuff.loadFromFile("sounds/191593__fins__menu-button.wav");
	menuBackBuff.loadFromFile("sounds/191592__fins__menu-button.wav");
	combo5Buff.loadFromFile("sounds/171641__fins__scale-c6.wav");
	combo10Buff.loadFromFile("sounds/171639__fins__scale-d6.wav");
	combo15Buff.loadFromFile("sounds/171646__fins__scale-e6.wav");
	combo20Buff.loadFromFile("sounds/171645__fins__scale-f6.wav");
	startBeepBuff.loadFromFile("sounds/13119__looppool__bell-blip.wav");

	garbAddSound.setBuffer(garbAddBuff);
	lineBlockSound.setBuffer(lineBlockBuff);
	menuSelSound.setBuffer(menuSelBuff);
	menuBackSound.setBuffer(menuBackBuff);
	combo5Sound.setBuffer(combo5Buff);
	combo10Sound.setBuffer(combo10Buff);
	combo15Sound.setBuffer(combo15Buff);
	combo20Sound.setBuffer(combo20Buff);
	startBeep1Sound.setBuffer(startBeepBuff);
	startBeep2Sound.setBuffer(startBeepBuff);

	startBeep2Sound.setPitch(1.5);
}

void soundBank::setEffectVolume(short vol) {
	garbAddSound.setVolume(vol);
	lineBlockSound.setVolume(vol);
	menuSelSound.setVolume(vol);
	menuBackSound.setVolume(vol);
	combo5Sound.setVolume(vol);
	combo10Sound.setVolume(vol);
	combo15Sound.setVolume(vol);
	combo20Sound.setVolume(vol);
	startBeep1Sound.setVolume(vol);
	startBeep2Sound.setVolume(vol);
}

void soundBank::setMusicVolume(short vol) {

}

void soundBank::setChatVolume(short vol) {
	
}