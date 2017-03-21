#ifndef SOUNDS_H
#define SOUNDS_H

#define SFML_NO_DEPRECATED_WARNINGS
#include <SFML/Audio.hpp>

class soundBank {
public:
	soundBank();

	sf::SoundBuffer comboTimeBuff;
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

	sf::Sound comboTimeSound;
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

	void comboTimeStart() { comboTimeSound.play(); }
	void comboTimeStop() { comboTimeSound.stop(); }
	void pieceDrop() { pieceDropSound.play(); }
	void lineClear() { lineClearSound.play(); }
	void garbAdd() { garbAddSound.play(); }
	void lineBlock() { lineBlockSound.play(); }
	void menuSel() { menuSelSound.play(); }
	void menuBack() { menuBackSound.play(); }
	void combo5() { combo5Sound.play(); }
	void combo8() { combo8Sound.play(); }
	void combo11() { combo11Sound.play(); }
	void combo13() { combo13Sound.play(); }
	void combo15() { combo15Sound.play(); }
	void combo17() { combo17Sound.play(); }
	void combo19() { combo19Sound.play(); }
	void combo21() { combo21Sound.play(); }
	void startBeep1() { startBeep1Sound.play(); }
	void startBeep2() { startBeep2Sound.play(); }

	void setMusicVolume(short vol);
	void setEffectVolume(short vol);
	void setChatVolume(short vol);
};

#endif
