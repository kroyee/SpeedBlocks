#include "sounds.h"

#ifdef __APPLE__
#include "ResourcePath.hpp"
#else
#include "EmptyResourcePath.h"
#endif

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

	pieceDropSound.setBuffer(pieceDropBuff);
	lineClearSound.setBuffer(lineClearBuff);
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
	alertSound.setBuffer(alertBuff);

	startBeep2Sound.setPitch(1.5);

	return "OK";
}

void soundBank::setEffectVolume(short vol) {
    pieceDropSound.setVolume(vol);
    lineClearSound.setVolume(vol);
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

void soundBank::setAlertVolume(short vol) {
	alertSound.setVolume(vol);
}