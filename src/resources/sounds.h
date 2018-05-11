#ifndef SOUNDS_H
#define SOUNDS_H

#include <SFML/Audio.hpp>

class soundBank {
public:
	soundBank();

	sf::SoundBuffer pieceDropBuff;
	sf::SoundBuffer lineClearBuff;
	sf::SoundBuffer garbAddBuff;
	sf::SoundBuffer lineBlockBuff;
	sf::SoundBuffer menuSelBuff;
	sf::SoundBuffer menuBackBuff;
	sf::SoundBuffer combo5Buff;
	sf::SoundBuffer combo8Buff;
	sf::SoundBuffer combo11Buff;
	sf::SoundBuffer combo13Buff;
	sf::SoundBuffer combo15Buff;
	sf::SoundBuffer combo17Buff;
	sf::SoundBuffer combo19Buff;
	sf::SoundBuffer combo21Buff;
	sf::SoundBuffer startBeepBuff;
	sf::SoundBuffer alertBuff;

	sf::Sound pieceDropSound;
	sf::Sound lineClearSound;
	sf::Sound garbAddSound;
	sf::Sound lineBlockSound;
	sf::Sound menuSelSound;
	sf::Sound menuBackSound;
	sf::Sound combo5Sound;
	sf::Sound combo8Sound;
	sf::Sound combo11Sound;
	sf::Sound combo13Sound;
	sf::Sound combo15Sound;
	sf::Sound combo17Sound;
	sf::Sound combo19Sound;
	sf::Sound combo21Sound;
	sf::Sound startBeep1Sound;
	sf::Sound startBeep2Sound;
	sf::Sound alertSound;

	std::vector<sf::Sound> soundList;

	std::string loadSounds();

	void playSound(int soundId);

	void setMusicVolume(int);
	void setEffectVolume(int vol);
	void setAlertVolume(int vol);
};

#endif
