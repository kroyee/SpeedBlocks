#include "GameDraw.h"
#include "GuiElements.h"
#include "Resources.h"
#include "optionSet.h"
#include "textures.h"
#include "GameSignals.h"

#include <iostream>
using std::cout;
using std::endl;

GameDraw::GameDraw(Resources& _res, GuiElements& _elems, bool& _drawMe) :
resources(_res),
guiElements(_elems),
drawMe(_drawMe) {
    Signals::FieldFinishedDrawing.connect([&](){ fieldsDone++; });
}

void GameDraw::draw() {
    if (status == 3) {
        resources.gfx->tGui.draw();
        resources.window.setActive(false);
        status = 4;
    }

    sf::Time current = resources.delayClock.getElapsedTime();
    /*if (status == 0 && current > nextDraw) {  // Experimental version where tgui was drawn in thread. SigSev.
        fieldsTotal=0;
        fieldsDone=0;
        if (drawMe) {
            Signals::MakeDrawCopy();
            fieldsTotal++;
        }
        for (auto& field : guiElements.gameFieldDrawer.fields)
            if (field.drawMe) {
                field.makeDrawCopy();
                fieldsTotal++;
            }

        //resources.window.setActive(false);
        status = 1;
        nextDraw+=resources.options->frameDelay;
    }*/
    if (status == 0) {
        fieldsTotal=1;
        fieldsDone=0;
        for (auto& field : guiElements.gameFieldDrawer.fields)
            if (field.drawMe) {
                field.makeDrawCopy();
                fieldsTotal++;
            }
        status = 1;
    }
    if (status == 1 && current > nextDraw) {
        if (drawMe)
            Signals::MakeDrawCopy();
        else
            fieldsDone++;
        while (nextDraw <= resources.delayClock.getElapsedTime())
            nextDraw+=resources.options->frameDelay;
        status = 2;
    }

    if (resources.delayClock.getElapsedTime() < nextUpdate) {
        sf::sleep(nextUpdate - resources.delayClock.getElapsedTime() - sf::microseconds(50));
        while (resources.delayClock.getElapsedTime() < nextUpdate) {}
    }
    while (nextUpdate <= resources.delayClock.getElapsedTime())
        nextUpdate += resources.options->inputDelay;

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
                internal = 2;
        	}
            else if (internal == 2) {
                if (fieldsDone == fieldsTotal)
                    internal = 3;
                else
                    sf::sleep(sf::seconds(0));
            }
            else if (internal == 3) {
                if (resources.gamestate != GameStates::MainMenu) {
                    if (resources.gamestate != GameStates::Spectating)
                        Signals::GameDrawSprite();
                    //lock gameFieldDrawer::fields mutex
                    guiElements.gameFieldDrawer.drawFields();
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