#ifndef SOUNDS_H
#define SOUNDS_H

#include <SFML/Audio.hpp>

class soundBank {
public:
	soundBank();
	
	sf::SoundBuffer garbAddBuff;
	sf::SoundBuffer lineBlockBuff;
	sf::SoundBuffer menuSelBuff;
	sf::SoundBuffer menuBackBuff;
	sf::SoundBuffer combo5Buff;
	sf::SoundBuffer combo10Buff;
	sf::SoundBuffer combo15Buff;
	sf::SoundBuffer combo20Buff;
	sf::SoundBuffer startBeepBuff;

	sf::Sound garbAddSound;
	sf::Sound lineBlockSound;
	sf::Sound menuSelSound;
	sf::Sound menuBackSound;
	sf::Sound combo5Sound;
	sf::Sound combo10Sound;
	sf::Sound combo15Sound;
	sf::Sound combo20Sound;
	sf::Sound startBeep1Sound;
	sf::Sound startBeep2Sound;

	void garbAdd() { garbAddSound.play(); }
	void lineBlock() { lineBlockSound.play(); }
	void menuSel() { menuSelSound.play(); }
	void menuBack() { menuBackSound.play(); }
	void combo5() { combo5Sound.play(); }
	void combo10() { combo10Sound.play(); }
	void combo15() { combo15Sound.play(); }
	void combo20() { combo20Sound.play(); }
	void startBeep1() { startBeep1Sound.play(); }
	void startBeep2() { startBeep2Sound.play(); }

	void setMusicVolume(short vol);
	void setEffectVolume(short vol);
	void setChatVolume(short vol);
};

#endif