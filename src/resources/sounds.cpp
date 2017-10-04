#include "sounds.h"
#include "GameSignals.h"

#ifdef __APPLE__
#include "ResourcePath.hpp"
#else
#include "EmptyResourcePath.h"
#endif

soundBank::soundBank(bool& _sound) : sound(_sound) {
	Signals::PlaySound.connect(&soundBank::playSound, this);
	Signals::SetEffectVolume.connect(&soundBank::setEffectVolume, this);
	Signals::SetMusicVolume.connect(&soundBank::setMusicVolume, this);
	Signals::SetAlertsVolume.connect(&soundBank::setAlertVolume, this);
}

sf::String soundBank::loadSounds() {
	if (!pieceDropBuff.loadFromFile(resourcePath() + "sounds/block.wav"))
		return "sounds/block.wav";
    if (!lineClearBuff.loadFromFile(resourcePath() + "sounds/clear.wav"))
    	return "sounds/clear.wav";
	if (!garbAddBuff.loadFromFile(resourcePath() + "sounds/173958__fins__failure.wav"))
		return "sounds/173958__fins__failure.wav";
	if (!lineBlockBuff.loadFromFile(resourcePath() + "sounds/350905__cabled-mess__jump-c-05.wav"))
		return "sounds/350905__cabled-mess__jump-c-05.wav";
	if (!menuSelBuff.loadFromFile(resourcePath() + "sounds/191593__fins__menu-button.wav"))
		return "sounds/191593__fins__menu-button.wav";
	if (!menuBackBuff.loadFromFile(resourcePath() + "sounds/191592__fins__menu-button.wav"))
		return "sounds/191592__fins__menu-button.wav";
	if (!combo5Buff.loadFromFile(resourcePath() + "sounds/171641__fins__scale-c6.wav"))
		return "sounds/171641__fins__scale-c6.wav";
	if (!combo8Buff.loadFromFile(resourcePath() + "sounds/171639__fins__scale-d6.wav"))
		return "sounds/171639__fins__scale-d6.wav";
	if (!combo11Buff.loadFromFile(resourcePath() + "sounds/171646__fins__scale-e6.wav"))
		return "sounds/171646__fins__scale-e6.wav";
	if (!combo13Buff.loadFromFile(resourcePath() + "sounds/171645__fins__scale-f6.wav"))
		return "sounds/171645__fins__scale-f6.wav";
	if (!combo15Buff.loadFromFile(resourcePath() + "sounds/171644__fins__scale-g6.wav"))
		return "sounds/171644__fins__scale-g6.wav";
	if (!combo17Buff.loadFromFile(resourcePath() + "sounds/171642__fins__scale-a6.wav"))
		return "sounds/171642__fins__scale-a6.wav";
	if (!combo19Buff.loadFromFile(resourcePath() + "sounds/171643__fins__scale-h6.wav"))
		return "sounds/171643__fins__scale-h6.wav";
	if (!combo21Buff.loadFromFile(resourcePath() + "sounds/171640__fins__scale-c7.wav"))
		return "sounds/171640__fins__scale-c7.wav";
	if (!startBeepBuff.loadFromFile(resourcePath() + "sounds/13119__looppool__bell-blip.wav"))
		return "sounds/13119__looppool__bell-blip.wav";
	if (!alertBuff.loadFromFile(resourcePath() + "sounds/274183__littlerobotsoundfactory__jingle-win-synth-04.wav"))
		return "sounds/274183__littlerobotsoundfactory__jingle-win-synth-04.wav";

	soundList.push_back(sf::Sound()); soundList.back().setBuffer(pieceDropBuff);
	soundList.push_back(sf::Sound()); soundList.back().setBuffer(lineClearBuff);
	soundList.push_back(sf::Sound()); soundList.back().setBuffer(garbAddBuff);
	soundList.push_back(sf::Sound()); soundList.back().setBuffer(lineBlockBuff);
	soundList.push_back(sf::Sound()); soundList.back().setBuffer(menuSelBuff);
	soundList.push_back(sf::Sound()); soundList.back().setBuffer(menuBackBuff);
	soundList.push_back(sf::Sound()); soundList.back().setBuffer(combo5Buff);
	soundList.push_back(sf::Sound()); soundList.back().setBuffer(combo8Buff);
	soundList.push_back(sf::Sound()); soundList.back().setBuffer(combo11Buff);
	soundList.push_back(sf::Sound()); soundList.back().setBuffer(combo13Buff);
	soundList.push_back(sf::Sound()); soundList.back().setBuffer(combo15Buff);
	soundList.push_back(sf::Sound()); soundList.back().setBuffer(combo17Buff);
	soundList.push_back(sf::Sound()); soundList.back().setBuffer(combo19Buff);
	soundList.push_back(sf::Sound()); soundList.back().setBuffer(combo21Buff);
	soundList.push_back(sf::Sound()); soundList.back().setBuffer(startBeepBuff);
	soundList.push_back(sf::Sound()); soundList.back().setBuffer(startBeepBuff); soundList.back().setPitch(1.5);
	soundList.push_back(sf::Sound()); soundList.back().setBuffer(alertBuff);

	return "OK";
}

void soundBank::playSound(int soundId) {
	if (sound)
		soundList[soundId].play();
}

void soundBank::setEffectVolume(int vol) {
	for (int i=0; i<16; i++)
		soundList[i].setVolume(vol);
}

void soundBank::setMusicVolume(int) {

}

void soundBank::setAlertVolume(int vol) {
	soundList[16].setVolume(vol);
}