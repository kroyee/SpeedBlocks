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

bool loadError(sf::String error) {
    if (error == "OK")
        return false;
    sf::RenderWindow window;
    window.create(sf::VideoMode(500, 400), "SpeedBlocks");
    window.clear();
    tgui::Gui gui(window);
    tgui::Label::Ptr errorMsg = tgui::Label::create();
    errorMsg->setText("Failed to load resources " + error + "\nSee that the file is there or reinstall the game.\n\nPress any key to quit.");
    errorMsg->setTextColor(sf::Color::White);
    errorMsg->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Center);
    errorMsg->setTextSize(35);
    errorMsg->setSize(500,400);
    gui.add(errorMsg);
    gui.draw();
    window.display();

    sf::Event event;
    while (1) {
        window.waitEvent(event);
        if (event.type == sf::Event::KeyPressed || event.type == sf::Event::Closed)
            break;
    }
    return true;
}

int main()
{
    // Initializing classes and loading resources

    sf::Font typewriter, printFont;
    if (!typewriter.loadFromFile(resourcePath() + "media/Kingthings Trypewriter 2.ttf")) {
        loadError("media/Kingthings Trypewriter 2.ttf");
        return false;
    }
    if (!printFont.loadFromFile(resourcePath() + "media/F25_Bank_Printer.ttf")) {
        loadError("media/F25_Bank_Printer.ttf");
        return false;
    }

    textures textureBase;
    if (loadError(textureBase.loadTextures()))
        return false;

    network net;

    soundBank sounds;
    if (loadError(sounds.loadSounds()))
        return false;

    gamePlay game(&textureBase, &sounds, printFont, &typewriter);

    game.field.setName(game.options.name);

    sounds.setEffectVolume(game.options.EffectVolume);
    sounds.setMusicVolume(game.options.MusicVolume);
    sounds.setChatVolume(game.options.ChatVolume);

    sf::RenderWindow window;
    if (game.options.fullscreen)
        window.create(game.options.modes[game.options.currentmode], "SpeedBlocks", sf::Style::Fullscreen);
    if (!window.isOpen()) {
        window.create(sf::VideoMode(960, 600), "SpeedBlocks");
        //window.create(sf::VideoMode(544, 340), "SpeedBlocks");
        game.options.fullscreen=false;
        game.options.currentmode=-1;
    }
    sf::View view(sf::FloatRect(0, 0, 960, 600));
    window.setView(view);
    window.setKeyRepeatEnabled(false);
    if (game.options.vSync)
        window.setVerticalSyncEnabled(true);

    UI gui(window, typewriter, printFont, game.options, sounds, game, net, textureBase);
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
            while (net.receiveData())
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
            default:
            break;
        }

        // Drawing to the screen

        current = frameClock.getElapsedTime();
        if (current > nextDraw || game.options.vSync) {
            if (game.drawMe && (gui.gamestate == Game || gui.gamestate == Replay)) {
                game.draw();
                game.drawMe=false;
            }
            if (game.preDrawMe && gui.gamestate == Replay) {
                game.preDraw();
                game.preDrawMe=false;
            }
            nextDraw+=game.options.frameDelay;
            window.draw(textureBase.background);
            if (gui.gamestate == CountDown || gui.gamestate == Game || gui.gamestate == GameOver || gui.gamestate == Replay) {
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
            sf::sleep(game.options.inputDelay - (current - lastFrame));
        if (nextDraw < current)
            nextDraw=current;

        // Performance output

        if (game.options.performanceOutput) {
            current = frameClock.getElapsedTime();
            if (current-lastFrame > longestFrame)
                longestFrame = current-lastFrame;
            frameCount++;

            if (current-secCount > sf::seconds(1)) {
                gui.frameRate.setString(to_string(frameRate));
                gui.frameRateColor = 255;
                if (frameRate < 40)
                    gui.frameRateColor = 0;
                else if (frameRate < 100)
                    gui.frameRateColor = (frameRate-40)*4.25;
                gui.inputRate.setString(to_string(frameCount));
                gui.inputRateColor = 255;
                if (frameCount < 255)
                    gui.inputRateColor = frameCount;
                gui.longestFrame.setString(to_string(longestFrame.asMilliseconds()));
                gui.longestFrameColor = 255;
                if (longestFrame.asMilliseconds() > 21)
                    gui.longestFrameColor = 0;
                else if (longestFrame.asMilliseconds() > 4)
                    gui.longestFrameColor = 255 - (longestFrame.asMilliseconds() - 4) * 15;
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