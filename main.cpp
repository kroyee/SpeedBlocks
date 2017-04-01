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

#define CLIENT_VERSION 1

void resizeWindow(sf::View& view, sf::Event& event) {
    float ratio;
    if ((float)event.size.width/event.size.height > 960.0/600) {
        ratio = (event.size.height * (960.0/600)) / event.size.width;
        view.setViewport(sf::FloatRect((1-ratio)/2.0, 0, ratio, 1));
    }
    else {
        ratio = (event.size.width / (960.0/600)) / event.size.height;
        view.setViewport(sf::FloatRect(0, (1-ratio)/2.0, 1, ratio));
    }
}

bool loadError(sf::String error) {
    if (error == "OK")
        return false;
    sf::RenderWindow window;
    window.create(sf::VideoMode(500, 400), "SpeedBlocks");
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

    srand(time(NULL));

    gamePlay game(&textureBase, &sounds, &typewriter);

    game.field.setName(game.options.name, printFont);

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

    while (window.isOpen())
    {
        sf::Event event;
        switch (gui.gamestate) {
            case MainMenu:
                while (window.pollEvent(event))
                {
                    gui.handleEvent(event);
                    if (event.type == sf::Event::Closed)
                        window.close();
                    else if (event.type == sf::Event::Resized && gui.options->currentmode == -1) {
                        resizeWindow(view, event);
                        window.setView(view);
                        gui.gui.setView(view);
                    }
                }
                if (gui.inroom)
                    gui.gamestate=GameOver;
                if (gui.quit) {
                    window.close();
                }
                if (gui.playonline)
                    while (net.receiveData())
                            gui.handlePacket();
            break;

            case CountDown:
                while (window.pollEvent(event)) {
                    gui.handleEvent(event);
                    if (event.type == sf::Event::Closed)
                        window.close();
                    else if (event.type == sf::Event::Resized && gui.options->currentmode == -1) {
                        resizeWindow(view, event);
                        window.setView(view);
                        gui.gui.setView(view);
                    }
                    else if (event.type == sf::Event::KeyPressed && !gui.chatFocused) {
                        if (event.key.code == game.options.right)
                            game.rKey=true;
                        else if (event.key.code == game.options.left)
                            game.lKey=true;
                        else if (event.key.code == game.options.chat)
                            gui.Chat();
                        else if (event.key.code == game.options.score)
                            gui.Score();
                        else if (event.key.code == game.options.away && gui.playonline) {
                            if (gui.away)
                                gui.unAway();
                            else
                                gui.goAway();
                        }
                    }
                    else if (event.type == sf::Event::KeyReleased) {
                        if (event.key.code == game.options.right)
                            game.rKey=false;
                        else if (event.key.code == game.options.left)
                            game.lKey=false;
                    }
                }
                if (gui.playonline) {
                    while (net.receiveData())
                            gui.handlePacket();
                    if (gui.startgame) {
                        gui.linesSent=0;
                        gui.garbageCleared=0;
                        gui.linesBlocked=0;
                        game.startGame();
                        gui.gamestate = Game;
                    }
                    if (gui.quit) {
                        gui.gamestate= MainMenu;
                        gui.leaveRoom();
                        gui.quit=false;
                    }
                    if (gui.startcount)
                        gui.startcount=false;
                }
                else if (gui.quit) {
                    gui.gamestate = MainMenu;
                    gui.mainMenu();
                    gui.quit=false;
                }
                else if (game.countDown()) {
                    game.startGame();
                    gui.gamestate = Game;
                }

                if (game.gameOver()) {
                    gui.gamestate = GameOver;
                    gui.startgame=false;
                    gui.startcount=false;
                    if (game.autoaway)
                        gui.goAway();
                    if (game.sendgameover)
                        gui.sendGameOver();
                    if (game.winner)
                        gui.sendGameWinner();
                }
            break;

            case Game:
                while (window.pollEvent(event))
                {
                    gui.handleEvent(event);
                    if (event.type == sf::Event::Closed)
                        window.close();
                    else if (event.type == sf::Event::Resized && gui.options->currentmode == -1) {
                        resizeWindow(view, event);
                        window.setView(view);
                        gui.gui.setView(view);
                    }
                    else if (event.type == sf::Event::KeyPressed && !gui.chatFocused) {
                        if (event.key.code == game.options.right)
                            game.mRKey();
                        else if (event.key.code == game.options.left)
                            game.mLKey();
                        else if (event.key.code == game.options.rcw)
                            game.rcw();
                        else if (event.key.code == game.options.rccw)
                            game.rccw();
                        else if (event.key.code == game.options.r180)
                            game.r180();
                        else if (event.key.code == game.options.down)
                            game.mDKey();
                        else if (event.key.code == game.options.hd)
                            game.hd();
                        else if (event.key.code == game.options.chat)
                            gui.Chat();
                        else if (event.key.code == game.options.score)
                            gui.Score();
                        else if (event.key.code == game.options.away && gui.playonline) {
                            if (gui.away)
                                gui.unAway();
                            else
                                gui.goAway();
                        }
                    }
                    else if (event.type == sf::Event::KeyReleased) {
                        if (event.key.code == game.options.right)
                            game.sRKey();
                        else if (event.key.code == game.options.left)
                            game.sLKey();
                        else if (event.key.code == game.options.down)
                            game.sDKey();
                    }
                }
                game.delayCheck();

                if (gui.playonline) {
                    while (net.receiveData())
                            gui.handlePacket();
                    if (gui.quit) {
                        gui.gamestate=MainMenu;
                        gui.leaveRoom();
                        gui.quit=false;
                        gui.startgame=false;
                    }
                    gui.sendGameData();
                }
                else if (gui.quit) {
                    gui.gamestate = MainMenu;
                    gui.mainMenu();
                    gui.quit=false;
                }

                if (game.gameOver()) {
                    gui.gamestate = GameOver;
                    gui.startgame=false;
                    if (game.autoaway)
                        gui.goAway();
                    if (game.sendgameover)
                        gui.sendGameOver();
                    if (game.winner)
                        gui.sendGameWinner();
                }

                if (gui.startcount) {
                    gui.gamestate=CountDown;
                    gui.startcount=false;
                    gui.startgame=false;
                    game.sRKey();
                    game.sLKey();
                    game.sDKey();
                }
            break;

            case GameOver:
                while (window.pollEvent(event))
                {
                    gui.handleEvent(event);
                    if (event.type == sf::Event::Closed)
                        window.close();
                    else if (event.type == sf::Event::Resized && gui.options->currentmode == -1) {
                        resizeWindow(view, event);
                        window.setView(view);
                        gui.gui.setView(view);
                    }
                    else if (event.type == sf::Event::KeyPressed && !gui.chatFocused) {
                        if (event.key.code == sf::Keyboard::Return && !gui.playonline) {
                            gui.setGameState(CountDown);
                            game.startCountdown();
                            game.gameover=false;
                        }
                        else if (event.key.code == game.options.chat)
                            gui.Chat();
                        else if (event.key.code == game.options.score)
                            gui.Score();
                        else if (event.key.code == game.options.away && gui.playonline) {
                            if (gui.away)
                                gui.unAway();
                            else
                                gui.goAway();
                        }
                    }
                }
                if (gui.playonline) {
                    while (net.receiveData())
                            gui.handlePacket();
                    if (gui.startcount) {
                        gui.gamestate=CountDown;
                        gui.startcount=false;
                        game.gameover=false;
                        game.sRKey();
                        game.sLKey();
                        game.sDKey();
                    }
                    if (gui.startgame) {
                        gui.linesSent=0;
                        gui.garbageCleared=0;
                        gui.linesBlocked=0;
                        game.startGame();
                        gui.gamestate = Game;
                    }
                    if (gui.quit) {
                        gui.gamestate= MainMenu;
                        gui.leaveRoom();
                        gui.quit=false;
                    }
                    if (game.winner)
                        gui.sendGameWinner();
                }
                else if (gui.quit) {
                    gui.gamestate = MainMenu;
                    gui.mainMenu();
                    gui.quit=false;
                }
            break;
        }

        current = frameClock.getElapsedTime();
        if (current > nextDraw || game.options.vSync) {
            if (game.drawMe && gui.gamestate == Game) {
                game.draw();
                game.drawMe=false;
            }
            nextDraw+=game.options.frameDelay;
            window.draw(textureBase.background);
            if (gui.gamestate == CountDown || gui.gamestate == Game || gui.gamestate == GameOver) {
                window.draw( game.field.sprite );
                gui.drawFields();
            }
            gui.gui.draw();
            if (gui.adjPieces)
                for (int i=0; i<7; i++)
                    window.draw(gui.piece[i]);
            window.display();
            frameRate++;
        }
        else if (!game.options.vSync)
            sf::sleep(game.options.inputDelay);
        if (nextDraw < current)
            nextDraw=current;

        current = frameClock.getElapsedTime();
        if (current-lastFrame > longestFrame)
            longestFrame = current-lastFrame;
        frameCount++;

        if (current-secCount > sf::seconds(1)) {
            cout << "Framerate: " << frameRate << " Framecount: " << frameCount << " longest: " << longestFrame.asMilliseconds() << endl;
            frameRate=0;
            frameCount=0;
            longestFrame=sf::seconds(0);
            secCount=current;
        }
        lastFrame=current;
    }

    game.options.saveOptions();

    return 0;
}