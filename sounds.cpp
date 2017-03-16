#include "sounds.h"
#include <iostream>
using namespace std;

soundBank::soundBank() {
	garbAddBuff.loadFromFile("sounds/173958__fins__failure.wav");
	lineBlockBuff.loadFromFile("sounds/350905__cabled-mess__jump-c-05.wav");
	menuSelBuff.loadFromFile("sounds/191593__fins__menu-button.wav");
	menuBackBuff.loadFromFile("sounds/191592__fins__menu-button.wav");
	combo5Buff.loadFromFile("sounds/171641__fins__scale-c6.wav");
	combo8Buff.loadFromFile("sounds/171639__fins__scale-d6.wav");
	combo11Buff.loadFromFile("sounds/171646__fins__scale-e6.wav");
	combo13Buff.loadFromFile("sounds/171645__fins__scale-f6.wav");
	combo15Buff.loadFromFile("sounds/171644__fins__scale-g6.wav");
	combo17Buff.loadFromFile("sounds/171642__fins__scale-a6.wav");
	combo19Buff.loadFromFile("sounds/171643__fins__scale-h6.wav");
	combo21Buff.loadFromFile("sounds/171640__fins__scale-c7.wav");
	startBeepBuff.loadFromFile("sounds/13119__looppool__bell-blip.wav");

	garbAddSound.setBuffer(garbAddBuff);
	lineBlockSound.setBuffer(lineBlockBuff);
	menuSelSound.setBuffer(menuSelBuff);
	menuBackSound.setBuffer(menuBackBuff);
	combo5Sound.setBuffer(combo5Buff);
	combo8Sound.setBuffer(combo8Buff);
	combo11Sound.setBuffer(combo11Buff);
	combo13Sound.setBuffer(combo13Buff);
	combo15Sound.setBuffer(combo15Buff);
	combo17Sound.setBuffer(combo17Buff);
	combo19Sound.setBuffer(combo19Buff);
	combo21Sound.setBuffer(combo21Buff);
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
	combo8Sound.setVolume(vol);
	combo11Sound.setVolume(vol);
	combo13Sound.setVolume(vol);
	combo15Sound.setVolume(vol);
	combo17Sound.setVolume(vol);
	combo19Sound.setVolume(vol);
	combo21Sound.setVolume(vol);
	startBeep1Sound.setVolume(vol);
	startBeep2Sound.setVolume(vol);
}

void soundBank::setMusicVolume(short vol) {

}

void soundBank::setChatVolume(short vol) {
	
}