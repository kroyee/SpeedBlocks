#include <SFML/Graphics.hpp>
#include "gamePlay.h"
#include "gui.h"
#include "GameOptions.h"
#include "PerformanceOutput.h"
#include "ReplayUI.h"
#include <iostream> // just here for quick and simple error testing, remove if you want
#include "AnimatedBackground.h"
#include "SlideMenu.h"
#include "optionSet.h"
#include "textures.h"
#include "network.h"
#include "GuiElements.h"
#include <string>
#include <cmath>
#include <thread>

using std::cout;
using std::endl;

#ifdef __APPLE__
#include "ResourcePath.hpp"
#elif __WIN32
#include <windows.h>
#endif
//#define DEBUG

#include "AI.h"

int main(int argc, char** argv)
{
    // Initializing classes and loading resources
    sf::RenderWindow window;

    Resources resources(window);
    if (!resources.init())
        return 0;

    gamePlay game(resources);

    #ifndef DEBUG
        if (resources.options->fullscreen)
            window.create(resources.options->modes[resources.options->currentmode], "SpeedBlocks", sf::Style::Fullscreen);
        if (!window.isOpen()) {
            window.create(sf::VideoMode(960, 600), "SpeedBlocks");
            resources.options->fullscreen=false;
            resources.options->currentmode=0;
        }
    #else
        window.create(sf::VideoMode(560,350), "SpeedBlocks");
    #endif
    sf::View view(sf::FloatRect(0, 0, 960, 600));
    window.setView(view);
    window.setKeyRepeatEnabled(false);
    if (resources.options->vSync)
        window.setVerticalSyncEnabled(true);

    #ifdef __WIN32
    HICON icon = LoadIcon( (HINSTANCE)GetWindowLong(window.getSystemHandle(), GWL_HINSTANCE), MAKEINTRESOURCE(128) );
    SendMessage(window.getSystemHandle(), WM_SETICON, ICON_SMALL, (LPARAM)icon);
    SendMessage(window.getSystemHandle(), WM_SETICON, ICON_BIG, (LPARAM)icon);
    #elif __APPLE__
    #else
    window.setIcon(128, 128, resources.gfx->icon->getPixelsPtr());
    #endif

    delete resources.gfx->icon;

    UI gui(window, game);

    resources.gfx->tGui.setView(view);

    game.rander.seedPiece(time(NULL)); // Make sure the seed is random-ish in case the client never connects
    game.rander.seedHole(time(NULL));

    sf::Clock frameClock;
    sf::Time current=sf::seconds(0), lastFrame=sf::seconds(0), nextDraw=sf::seconds(0), nextUpdate=sf::seconds(0);

    // Intro

    bool intro=true;
    while (intro) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::KeyPressed)
                intro=false;
            if (event.type == sf::Event::MouseButtonPressed)
                intro=false;
            if (event.type == sf::Event::Closed) {
                window.close();
                intro=false;
            }
        }
        float timing = frameClock.getElapsedTime().asMilliseconds() / 100.0;
        if (timing > 10) {
            timing=10;
            intro=false;
        }
        float posX = 300 - timing*30;
        float posY = 280-12*pow(timing-5, 2);
        float scale = 1.5 - timing*0.05;
        if (posX < 0)
            posX=0;
        if (scale < 1)
            scale=1;
        resources.gfx->logo.setPosition(posX, posY);
        resources.gfx->logo.setScale(scale, scale);
        gui.guiElements->animatedBackground.draw(window, gui.delayClock.getElapsedTime());
        window.draw(resources.gfx->logo);
        window.display();

        sf::sleep(sf::milliseconds(20));
    }

    // The main-loop

    Population pop(resources);
    
    /*if (argc < 2) {
        cout << "Usage: SpeedBlocks [filename] | [stack|downstack [size]]\n";
        cout << "filename: Specify a population file to load\n";
        cout << "stack: Start stack training with a new randomized population with [size]\n";
        cout << "downstack: Start downstack traning with a new randomized population with [size]" << endl;
        window.close();
    }
    else {
        std::string argument(argv[1]);
        if (argument == "stack") {
            int size=1000;
            if (argc < 3)
                cout << "No size specified, using default 1000" << endl;
            else {
                try{
                    size = std::stoi(argv[2]);
                } catch (...) {
                    cout << "Invalid size, using default 1000" << endl;
                }
            }
            pop.ai.mode = Mode::Stack;
            pop.makePopulation(size);
        }
        else if (argument == "downstack") {
            int size=1000;
            if (argc < 3)
                cout << "No size specified, using default 1000" << endl;
            else {
                try{
                    size = std::stoi(argv[2]);
                } catch (...) {
                    cout << "Invalid size, using default 1000" << endl;
                }
            }
            pop.ai.mode = Mode::Downstack;
            pop.makePopulation(size);
        }
        else {
            if (!pop.loadPopulation(argv[1])) {
                cout << "Does the file exist?" << endl;
                window.close();
            }
        }
    }

    if (window.isOpen()) {
        while (!pop.quit) {
            cout << "Starting cycle " << pop.cycleCount << endl;
            if (pop.populationCycle(100)) {
                pop.evolvePopulation();
                pop.cycleCount++;
                pop.savePopulation();
            }
        }

        pop.savePopulation();
    }*/

    pop.makePopulation(1);

    while (window.isOpen())
    {
        sf::Event event;

        while (window.pollEvent(event)) {
            if (gui.handleEvent(event))
                game.handleEvent(event);

            if (event.type == sf::Event::KeyPressed)
                if (event.key.code == sf::Keyboard::P)
                    pop.showBest();
        }

        if (resources.playonline)
            while (resources.net->receiveData()) {}

        gui.delayCheck();
        
        switch (gui.gamestate) {
            case GameStates::CountDown:
                if (!resources.playonline)
                    if (game.countDown())
                        gui.setGameState(GameStates::Game);
            break;

            case GameStates::Game:
            case GameStates::Practice:
                game.delayCheck();
            break;

            case GameStates::Replay:
                if (game.playReplay())
                    gui.setGameState(GameStates::GameOver);
            break;

            default:
            break;
        }

        // Drawing to the screen

        current = frameClock.getElapsedTime();
        if (current > nextDraw || game.options.vSync) {
            if (game.drawMe && (gui.gamestate == GameStates::Game || gui.gamestate == GameStates::Replay || gui.gamestate == GameStates::Practice)) {
                game.draw();
                game.drawMe=false;
            }
            nextDraw+=game.options.frameDelay;
            gui.guiElements->animatedBackground.draw(window, gui.delayClock.getElapsedTime());
            if (gui.gamestate != GameStates::MainMenu && gui.gamestate != GameStates::Spectating)
                window.draw( game.field.sprite );
            if (gui.guiElements->gameFieldDrawer.isVisible())
                gui.guiElements->gameFieldDrawer.drawFields();
            resources.gfx->tGui.draw();
            window.draw(pop.ai.field.sprite);
            window.display();
            gui.guiElements->performanceOutput.frameRate++;
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

        gui.guiElements->performanceOutput.update(frameClock.getElapsedTime(), lastFrame);

        lastFrame=current;
    }

    // Things to do before the game turns off

    game.options.saveOptions();
    if (resources.restart) {
        #ifdef _WIN32
            std::thread relaunch([](){ system("start SpeedBlocks.exe"); });
        #elif __APPLE__
            std::thread relaunch([](){
                std::string cmd = "open " + resourcePath() + "../../../SpeedBlocks.app";
                system(cmd.c_str());
            });
        #else
            std::thread relaunch([](){ system("./SpeedBlocks"); });
        #endif
        relaunch.detach();
    }

    return 0;
}