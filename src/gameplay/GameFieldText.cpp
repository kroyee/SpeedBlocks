#include "GameFieldText.h"
#include "Resources.h"
using std::to_string;

GameFieldText::GameFieldText(Resources& _resources) : resources(_resources) {
	nameTag.setFont(resources.gfx.typewriter);
    nameTag.setCharacterSize(32);
    readyText.setFont(resources.gfx.typewriter);
    readyText.setCharacterSize(32);
    readyText.setPosition(100, 75); readyText.setString("Ready");
    awayText.setFont(resources.gfx.typewriter);
    awayText.setCharacterSize(48);
    awayText.setPosition(110, 150); awayText.setString("Away");
    positionText.setFont(resources.gfx.typewriter);
    positionText.setCharacterSize(48);
    positionText.setPosition(130, 200);
    countdownText.setFont(resources.gfx.typewriter);
    countdownText.setCharacterSize(96);
    countdownText.setColor(sf::Color::White);
    countdownText.setPosition(130,210);

    comboText.setFont(resources.gfx.typewriter); comboText.setString("0");
    pendingText.setFont(resources.gfx.typewriter); pendingText.setString("0");
    bpmText.setFont(resources.gfx.typewriter); bpmText.setString("0");
    comboText.setCharacterSize(48);
    comboText.setColor(sf::Color::White);
    comboText.setPosition(360,320);
    pendingText.setCharacterSize(48);
    pendingText.setColor(sf::Color::White);
    pendingText.setPosition(360,500);
    bpmText.setCharacterSize(48);
    bpmText.setColor(sf::Color::White);
    bpmText.setPosition(360, 420);
    gameOverText.setFont(resources.gfx.typewriter);
    gameOverText.setPosition(50,250);
    gameOverText.setCharacterSize(48);

    sets.setFont(resources.gfx.typewriter);
    sets.setCharacterSize(24);
    sets.setColor(sf::Color::White);
    sets.setPosition(355,180);
    sets.setString("Sets");
    rounds.setFont(resources.gfx.typewriter);
    rounds.setCharacterSize(24);
    rounds.setColor(sf::Color::White);
    rounds.setPosition(340,230);
    rounds.setString("Rounds");
    p1_sets.setFont(resources.gfx.typewriter);
    p1_sets.setCharacterSize(18);
    p1_sets.setColor(sf::Color::White);
    p1_sets.setPosition(340,210);
    p1_sets.setString("0");
    p2_sets.setFont(resources.gfx.typewriter);
    p2_sets.setCharacterSize(18);
    p2_sets.setColor(sf::Color::White);
    p2_sets.setPosition(410,210);
    p2_sets.setString("0");
    p1_rounds.setFont(resources.gfx.typewriter);
    p1_rounds.setCharacterSize(18);
    p1_rounds.setColor(sf::Color::White);
    p1_rounds.setPosition(340,260);
    p1_rounds.setString("0");
    p2_rounds.setFont(resources.gfx.typewriter);
    p2_rounds.setCharacterSize(18);
    p2_rounds.setColor(sf::Color::White);
    p2_rounds.setPosition(410,260);
    p2_rounds.setString("0");

    comboTimer.setPosition(315, 290);
    comboTimer.setFillColor(sf::Color(255,0,0));
    setComboTimer(0);

    combo = 0; pending = 0; bpm = 0; position = 0; countdown = 0; gameover = 0;
    away = false; ready = false; results = false;
}

GameFieldText::GameFieldText(const GameFieldText& text) : resources(text.resources) {
	nameTag=text.nameTag; readyText=text.readyText; awayText=text.awayText; positionText = text.positionText;
	countdownText = text.countdownText; comboText = text.comboText; pendingText = text.pendingText;
	bpmText = text.bpmText; gameOverText = text.gameOverText;

	comboTimer.setPosition(315, 240);
    comboTimer.setFillColor(sf::Color(255,0,0));
    setComboTimer(0);

    combo = 0; pending = 0; bpm = 0; position = 0; countdown = 0; gameover = 0;
    away = false; ready = false; results = false;
}

void GameFieldText::setName(const sf::String& n) {
    name = n;
    nameTag.setString(n);
    short x = (310-nameTag.getLocalBounds().width)/2;
    if (x<0)
        x=0;
    nameTag.setPosition(x, 555);
}

void GameFieldText::setPosition(const sf::Int8 _position) {
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

void GameFieldText::setCountdown(const sf::Int8 _countdown) {
	countdown = _countdown;
	if (countdown)
		countdownText.setString(to_string(countdown));
}

void GameFieldText::setGameover(const sf::Int8 _gameover) {
	gameover = _gameover;
	if (gameover == 1)
		gameOverText.setString("Game Over");
	else if (gameover == 2)
		gameOverText.setString("Winner");
}

const float PI = 3.14159265;
bool GameFieldText::setComboTimer(sf::Uint8 count) {
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

void GameFieldText::setBpm(const sf::Uint16 _bpm) {
	if (bpm != _bpm) {
		bpm = _bpm;
		bpmText.setString(to_string(bpm));
	}
}

void GameFieldText::setCombo(const sf::Uint8 _combo) {
	if (combo != _combo) {
		combo = _combo;
		comboText.setString(to_string(combo));
	}
}

void GameFieldText::setPending(const sf::Uint8 _pending) {
	if (pending != _pending) {
		pending = _pending;
		comboText.setString(to_string(pending));
	}
}

void GameFieldText::setResults(sf::Uint16 myId) {
    results = true;
    sf::Uint8 _p1_sets, _p2_sets, _p1_rounds, _p2_rounds;
    sf::Uint16 p1_id, p2_id;
    resources.net.packet >> p1_id >> p2_id >> _p1_sets >> _p2_sets >> _p1_rounds >> _p2_rounds;

    if (p1_id == myId) {
        p1_sets.setString(to_string(_p1_sets));
        p2_sets.setString(to_string(_p2_sets));
        p1_rounds.setString(to_string(_p1_rounds));
        p2_rounds.setString(to_string(_p2_rounds));
    }
    else {
        p2_sets.setString(to_string(_p1_sets));
        p1_sets.setString(to_string(_p2_sets));
        p2_rounds.setString(to_string(_p1_rounds));
        p1_rounds.setString(to_string(_p2_rounds));
    }

    if (_p1_rounds == 255) {
        p1_rounds.setString("Game Over");
        p2_rounds.setString("");
    }
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

    if (results) {
        texture->draw(sets);
        texture->draw(rounds);
        texture->draw(p1_sets);
        texture->draw(p2_sets);
        texture->draw(p1_rounds);
        texture->draw(p2_rounds);
    }
}