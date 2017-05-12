#include <SFML/Graphics.hpp>
#include "gamePlay.h"
#include "textures.h"
#include "sounds.h"
#include "gui.h"
#include "network.h"
#include "packetcompress.h"
#include <iostream> // just here for quick and simple error testing, remove if you want
#include <TGUI/TGUI.hpp>
#include <sstream>

#ifdef __APPLE__
#include "ResourcePath.hpp"
#else
#include "EmptyResourcePath.h"
#endif

using namespace std;

#define CLIENT_VERSION 2

int main()
{
    // Initializing classes and loading resources
    Resources resources;
    if (!resources.init())
        return 0;

    gamePlay game(resources);

    sf::RenderWindow window;
    if (resources.options.fullscreen)
        window.create(resources.options.modes[resources.options.currentmode], "SpeedBlocks", sf::Style::Fullscreen);
    if (!window.isOpen()) {
        window.create(sf::VideoMode(960, 600), "SpeedBlocks");
        //window.create(sf::VideoMode(544, 340), "SpeedBlocks");
        resources.options.fullscreen=false;
        resources.options.currentmode=-1;
    }
    sf::View view(sf::FloatRect(0, 0, 960, 600));
    window.setView(view);
    window.setKeyRepeatEnabled(false);
    if (resources.options.vSync)
        window.setVerticalSyncEnabled(true);

    UI gui(window, game);
    gui.clientVersion = CLIENT_VERSION;

    gui.gui.setView(view);

    sf::Clock frameClock;
    sf::Time current=sf::seconds(0), nextDraw=sf::seconds(0);

    sf::Time lastFrame=sf::seconds(0), longestFrame=sf::seconds(0), secCount=sf::seconds(0);
    int frameCount=0, frameRate=0;

    game.rander.seedPiece(time(NULL)); // Make sure the seed is random-ish in case the client never connects

    // The main-loop

    while (window.isOpen())
    {
        sf::Event event;

        while (window.pollEvent(event))
            gui.handleEvent(event);

        if (gui.playonline)
            while (resources.net.receiveData())
                gui.handlePacket();

        gui.delayCheck();
        
        switch (gui.gamestate) {
            case CountDown:
                if (!gui.playonline)
                    if (game.countDown()) {
                        game.startGame();
                        gui.gamestate = Game;
                    }

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
            if (game.preDrawMe && gui.gamestate == Replay) {
                game.preDraw();
                game.preDrawMe=false;
            }
            nextDraw+=game.options.frameDelay;
            window.draw(resources.gfx.background);
            if (gui.gamestate != MainMenu) {
                window.draw( game.field.sprite );
                gui.drawFields();
            }
            if (gui.adjPieces)
                for (int i=0; i<7; i++)
                    window.draw(gui.piece[i]);
            gui.gui.draw();
            if (game.options.performanceOutput)
                gui.drawPerformanceOutput();
            window.display();
            frameRate++;
        }
        current = frameClock.getElapsedTime();
        if (game.options.inputDelay - (current - lastFrame) > sf::microseconds(200))
            sf::sleep(game.options.inputDelay - (current - lastFrame) - sf::microseconds(100));
        while (game.options.inputDelay - (current - lastFrame) > sf::microseconds(0)) {
            current = frameClock.getElapsedTime();
        }
        if (nextDraw < current)
            nextDraw=current;

        // Performance output

        if (game.options.performanceOutput) {
            current = frameClock.getElapsedTime();
            if (current-lastFrame > longestFrame)
                longestFrame = current-lastFrame;
            frameCount++;

            if (current-secCount > sf::seconds(1)) {
                gui.setFrameRate(frameRate);
                gui.setInputRate(frameCount);
                gui.setLongestFrame(longestFrame);
                frameRate=0;
                frameCount=0;
                longestFrame=sf::seconds(0);
                secCount=current;
            }
        }
        lastFrame=current;
    }

    // Things to do before the game turns off

    game.options.saveOptions();

    return 0;
}