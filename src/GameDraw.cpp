#include "GameDraw.h"
#include "GuiElements.h"
#include "Resources.h"
#include "Options.h"
#include "Textures.h"
#include "GameSignals.h"

static auto& MakeDrawCopy = Signal<void>::get("MakeDrawCopy");
static auto& GameDrawTexture = Signal<void>::get("GameDraw");
static auto& GameDrawSprite = Signal<void>::get("GameDrawSprite");

GameDraw::GameDraw(Resources& _res, GuiElements& _elems, bool& _drawMe) :
resources(_res),
guiElements(_elems),
drawMe(_drawMe) {}

void GameDraw::draw() {
    if (status == 3) {
        resources.gfx->tGui.draw();
        resources.window.setActive(false);
        status = 4;
    }

    sf::Time current = resources.delayClock.getElapsedTime();
    if (status == 0) {
        for (auto& field : guiElements.gameFieldDrawer.fields)
            if (field.drawMe)
                field.makeDrawCopy();
        status = 1;
    }
	static sf::Time& frameDelay = Options::get<sf::Time>("framedelay");
    if (status == 1 && current > nextDraw) {
        if (drawMe)
            MakeDrawCopy();
        while (nextDraw <= resources.delayClock.getElapsedTime())
            nextDraw+=frameDelay;
        status = 2;
    }

	static sf::Time& inputDelay = Options::get<sf::Time>("inputdelay");
    if (resources.delayClock.getElapsedTime() < nextUpdate) {
        sf::sleep(nextUpdate - resources.delayClock.getElapsedTime() - sf::microseconds(50));
        while (resources.delayClock.getElapsedTime() < nextUpdate) {}
    }
    while (nextUpdate <= resources.delayClock.getElapsedTime())
        nextUpdate += inputDelay;

    guiElements.performanceOutput.update(resources.delayClock.getElapsedTime(), current);
}

void GameDraw::init() {
	status = 0;
	drawThread = std::thread(&GameDraw::drawThreadLoop, this);
    resources.delayClock.restart();
    nextDraw = sf::seconds(0);
    nextUpdate = sf::seconds(0);
}

void GameDraw::quit() {
    status = 5;
    if (drawThread.joinable())
        drawThread.join();
}

void GameDraw::drawThreadLoop() {
    uint8_t internal=1;
    while (status != 5) {
        if (status == 1 || status == 2) {
        	if (internal == 1) {
                guiElements.animatedBackground.draw(resources.window, resources.delayClock.getElapsedTime());
                if (resources.gamestate != GameStates::MainMenu)
                    while (status != 2 && guiElements.gameFieldDrawer.drawNextField()) {}
                guiElements.gameFieldDrawer.drawFields();
                internal = 2;
        	}
            if (internal == 2 && status == 2) {
                if (resources.gamestate != GameStates::MainMenu && resources.gamestate != GameStates::Spectating) {
                    GameDrawTexture();
                    GameDrawSprite();
                }
                resources.window.setActive(false);
                status = 3;
            }
        }
    	else if (status == 4) { // display window
            resources.window.display();
            guiElements.performanceOutput.frameRate++;
            status = 0;
            internal = 1;
    	}
        sf::sleep(sf::seconds(0));
    }
}
