#include <SFML/Graphics.hpp>
#include "gamePlay.h"
#include "gui.h"
#include "GameOptions.h"
#include "PerformanceOutput.h"
#include "ReplayUI.h"
#include <iostream> // just here for quick and simple error testing, remove if you want
#include "machineid.h"
#include "AnimatedBackground.h"
#include <string>

using std::cout;
using std::endl;

#define CLIENT_VERSION 8
//#define DEBUG

int main()
{
    // Initializing classes and loading resources
    Resources resources;
    if (!resources.init())
        return 0;

    gamePlay game(resources);

    sf::RenderWindow window;
    AnimatedBackground background(7);

    #ifndef DEBUG
        if (resources.options.fullscreen)
            window.create(resources.options.modes[resources.options.currentmode], "SpeedBlocks", sf::Style::Fullscreen);
        if (!window.isOpen()) {
            window.create(sf::VideoMode(960, 600), "SpeedBlocks");
            resources.options.fullscreen=false;
            resources.options.currentmode=-1;
        }
    #else
        window.create(sf::VideoMode(560,350), "SpeedBlocks");
    #endif
    sf::View view(sf::FloatRect(0, 0, 960, 600));
    window.setView(view);
    window.setKeyRepeatEnabled(false);
    if (resources.options.vSync)
        window.setVerticalSyncEnabled(true);

    window.setIcon(128, 128, resources.gfx.icon->getPixelsPtr());
    delete resources.gfx.icon;

    UI gui(window, game);
    gui.clientVersion = CLIENT_VERSION;

    gui.tGui.setView(view);

    sf::Clock frameClock;
    sf::Time current=sf::seconds(0), lastFrame=sf::seconds(0), nextDraw=sf::seconds(0), nextUpdate=sf::seconds(0);

    game.rander.seedPiece(time(NULL)); // Make sure the seed is random-ish in case the client never connects
    game.rander.seedHole(time(NULL));

    // The main-loop

    while (window.isOpen())
    {
        sf::Event event;

        gui.dontForwardToChat=false;
        while (window.pollEvent(event))
            gui.handleEvent(event);

        if (gui.playonline)
            while (resources.net.receiveData())
                gui.handlePacket();

        gui.delayCheck();
        
        switch (gui.gamestate) {
            case CountDown:
                if (!gui.playonline)
                    if (game.countDown())
                        gui.setGameState(Game);

                if (game.gameOver())
                    gui.setGameState(GameOver);
            break;

            case Game:
                game.delayCheck();

                if (gui.playonline)
                    gui.sendGameData();

                if (game.gameOver())
                    gui.setGameState(GameOver);
            break;

            case GameOver:
                if (gui.playonline)
                    if (game.winner)
                        gui.sendGameWinner();
            break;
            case Replay:
                if (game.playReplay())
                    gui.setGameState(GameOver);
                gui.replayUI->update();
            break;

            case Practice:
                game.delayCheck();

                if (game.gameOver())
                    gui.setGameState(GameOver);
            break;

            default:
            break;
        }

        // Drawing to the screen

        current = frameClock.getElapsedTime();
        if (current > nextDraw || game.options.vSync) {
            if (game.drawMe && (gui.gamestate == Game || gui.gamestate == Replay || gui.gamestate == Practice)) {
                game.draw();
                game.drawMe=false;
            }
            nextDraw+=game.options.frameDelay;
            window.draw(resources.gfx.background);
            //background.draw(window, current);
            if (gui.gamestate != MainMenu && gui.gamestate != Spectating)
                window.draw( game.field.sprite );
            if (gui.gameFieldDrawer.isVisible())
                gui.gameFieldDrawer.drawFields();
            if (gui.gameOptions->GenOpt->isVisible())
                for (int i=0; i<7; i++)
                    window.draw(gui.gameOptions->piece[i]);
            gui.tGui.draw();
            window.display();
            gui.performanceOutput->frameRate++;
        }
        if (frameClock.getElapsedTime() < nextUpdate) {
            sf::sleep(nextUpdate - frameClock.getElapsedTime() - sf::microseconds(50));
            while (frameClock.getElapsedTime() < nextUpdate) {}
        }
        nextUpdate += game.options.inputDelay;
        if (nextUpdate < current)
            nextUpdate = current;
        if (nextDraw < current)
            nextDraw=current;

        gui.performanceOutput->update(frameClock.getElapsedTime(), lastFrame);

        lastFrame=current;
    }

    // Things to do before the game turns off

    game.options.saveOptions();

    return 0;
}