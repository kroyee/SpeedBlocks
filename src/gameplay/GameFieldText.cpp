#include "GameFieldText.h"
#include "Resources.h"
#include "Textures.h"
#include <cmath>
using std::to_string;

GameFieldText::GameFieldText(Resources& _resources) : resources(_resources) {
		nameTag.setFont(resources.gfx->font("typewriter"));
    nameTag.setCharacterSize(32);
    readyText.setFont(resources.gfx->font("typewriter"));
    readyText.setCharacterSize(32);
    readyText.setPosition(100, 75); readyText.setString("Ready");
    awayText.setFont(resources.gfx->font("typewriter"));
    awayText.setCharacterSize(48);
    awayText.setPosition(110, 150); awayText.setString("Away");
    positionText.setFont(resources.gfx->font("typewriter"));
    positionText.setCharacterSize(48);
    positionText.setPosition(130, 200);
    countdownText.setFont(resources.gfx->font("typewriter"));
    countdownText.setCharacterSize(96);
    countdownText.setPosition(130,210);

    comboText.setFont(resources.gfx->font("typewriter")); comboText.setString("0");
    pendingText.setFont(resources.gfx->font("typewriter")); pendingText.setString("0");
    bpmText.setFont(resources.gfx->font("typewriter")); bpmText.setString("0");
    comboText.setCharacterSize(48);
    comboText.setPosition(360,320);
    pendingText.setCharacterSize(48);
    pendingText.setPosition(360,500);
    bpmText.setCharacterSize(48);
    bpmText.setPosition(360, 420);
    gameOverText.setFont(resources.gfx->font("typewriter"));
    gameOverText.setPosition(50,250);
    gameOverText.setCharacterSize(48);

    comboTimer.setPosition(315, 290);
    comboTimer.setFillColor(sf::Color(255,0,0));
    setComboTimer(0);

    combo = 0; pending = 0; bpm = 0; position = 0; countdown = 0; gameover = 0;
    away = false; ready = false;

    setColor(resources.gfx->color("GameFieldText"));
}

GameFieldText::GameFieldText(const GameFieldText& text) : resources(text.resources) {
	nameTag=text.nameTag; readyText=text.readyText; awayText=text.awayText; positionText = text.positionText;
	countdownText = text.countdownText; comboText = text.comboText; pendingText = text.pendingText;
	bpmText = text.bpmText; gameOverText = text.gameOverText;

	comboTimer.setPosition(315, 240);
    comboTimer.setFillColor(sf::Color(255,0,0));
    setComboTimer(0);

    combo = 0; pending = 0; bpm = 0; position = 0; countdown = 0; gameover = 0;
    away = false; ready = false;
}

void GameFieldText::setName(const std::string& n) {
    std::lock_guard<std::mutex> mute(fieldTextMutex);
    name = n;
    nameTag.setString(n);
    short x = (310-nameTag.getLocalBounds().width)/2;
    if (x<0)
        x=0;
    nameTag.setPosition(x, 555);
}

void GameFieldText::setPosition(const int8_t _position) {
    std::lock_guard<std::mutex> mute(fieldTextMutex);
	position = _position;
	if (position == 1)
        positionText.setString("1st");
    else if (position == 2)
        positionText.setString("2nd");
    else if (position == 3)
        positionText.setString("3rd");
    else if (position)
        positionText.setString(to_string((int)position) + "th");
}

void GameFieldText::setCountdown(const int8_t _countdown) {
    std::lock_guard<std::mutex> mute(fieldTextMutex);
	countdown = _countdown;
	if (countdown)
		countdownText.setString(to_string(countdown));
}

void GameFieldText::setGameover(const int8_t _gameover) {
    std::lock_guard<std::mutex> mute(fieldTextMutex);
	gameover = _gameover;
	if (gameover == 1)
		gameOverText.setString("Game Over");
	else if (gameover == 2)
		gameOverText.setString("Winner");
}

const float PI = 3.14159265;
bool GameFieldText::setComboTimer(uint8_t count) {
    std::lock_guard<std::mutex> mute(fieldTextMutex);
    if (count>100)
        count=100;
    if (comboTimer.getPointCount() == static_cast<unsigned int>(count+2))
        return false;
    comboTimer.setPointCount(count+2);

    comboTimer.setPoint(0, sf::Vector2f(60, 60));
    for (int x=1; x<(count+2); x++)
        comboTimer.setPoint(x, sf::Vector2f(60 + 60*cos((PI*2)/100 * (x-26)), 60 + 60*sin((PI*2)/100 * (x-26) )));

    return true;
}

void GameFieldText::setBpm(const uint16_t _bpm) {
    std::lock_guard<std::mutex> mute(fieldTextMutex);
	if (bpm != _bpm) {
		bpm = _bpm;
		bpmText.setString(to_string(bpm));
	}
}

void GameFieldText::setCombo(const uint8_t _combo) {
    std::lock_guard<std::mutex> mute(fieldTextMutex);
	if (combo != _combo) {
		combo = _combo;
		comboText.setString(to_string(combo));
	}
}

void GameFieldText::setPending(const uint8_t _pending) {
    std::lock_guard<std::mutex> mute(fieldTextMutex);
	if (pending != _pending) {
		pending = _pending;
		pendingText.setString(to_string(pending));
	}
}

void GameFieldText::setColor(sf::Color color) {
    std::lock_guard<std::mutex> mute(fieldTextMutex);
    nameTag.setFillColor(color);
    awayText.setFillColor(color);
    positionText.setFillColor(color);
    readyText.setFillColor(color);
    comboText.setFillColor(color);
    pendingText.setFillColor(color);
    countdownText.setFillColor(color);
    bpmText.setFillColor(color);
    gameOverText.setFillColor(color);

    nameTag.setOutlineColor(color);
    awayText.setOutlineColor(color);
    positionText.setOutlineColor(color);
    readyText.setOutlineColor(color);
    comboText.setOutlineColor(color);
    pendingText.setOutlineColor(color);
    countdownText.setOutlineColor(color);
    bpmText.setOutlineColor(color);
    gameOverText.setOutlineColor(color);
}

void GameFieldText::clear() {
    setComboTimer(0);
    setCombo(0);
    setPending(0);
    setBpm(0);
    setPosition(0);
    setCountdown(0);
    setGameover(0);
    ready=false;
}

void GameFieldText::drawText() {
    std::lock_guard<std::mutex> mute(fieldTextMutex);
    if (away)
        texture->draw(awayText);
    if (position)
        texture->draw(positionText);
    if (ready)
    	texture->draw(readyText);
    if (countdown)
		texture->draw(countdownText);
	if (gameover)
		texture->draw(gameOverText);

    texture->draw(nameTag);
    texture->draw(comboTimer);
    texture->draw(comboText);
    texture->draw(pendingText);
    texture->draw(bpmText);
}
