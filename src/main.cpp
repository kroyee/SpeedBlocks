#include <SFML/Graphics.hpp>
#include "gamePlay.h"
#include "gui.h"
#include "GameOptions.h"
#include "PerformanceOutput.h"
#include "ReplayUI.h"
#include "AnimatedBackground.h"
#include "SlideMenu.h"
#include "Options.h"
#include "Textures.h"
#include "network.h"
#include "GuiElements.h"
#include "GameDraw.h"
#include "TaskQueue.h"
#include <string>
#include <cmath>
#include <thread>

#ifdef __APPLE__
#include "ResourcePath.hpp"
#elif __WIN32
#include <windows.h>
#else
#include <X11/Xlib.h>
#endif
//#define DEBUG

int main()
{
    #ifdef __APPLE__
    #elif __WIN32
    #else
    XInitThreads();
    #endif
    // Initializing classes and loading resources
    sf::RenderWindow window;

    Resources resources(window);
    if (!resources.init())
        return 0;

    gamePlay game(resources);

    #ifndef DEBUG
        if (Options::get<bool>("fullscreen"))
            window.create(Options::get<std::vector<sf::VideoMode>>("modes")[Options::get<short>("currentmode")], "SpeedBlocks", sf::Style::Fullscreen);
        if (!window.isOpen()) {
            window.create(sf::VideoMode(960, 600), "SpeedBlocks");
            Options::get<bool>("fullscreen")=false;
            Options::get<short>("currentmode")=0;
        }
    #else
        window.create(sf::VideoMode(560,350), "SpeedBlocks");
    #endif
    sf::View view(sf::FloatRect(0, 0, 960, 600));
    window.setView(view);
    window.setKeyRepeatEnabled(false);
    if (Options::get<bool>("vsync"))
        window.setVerticalSyncEnabled(true);

    #ifdef __WIN32
    HICON icon = LoadIcon( (HINSTANCE)GetWindowLong(window.getSystemHandle(), GWL_HINSTANCE), MAKEINTRESOURCE(128) );
    SendMessage(window.getSystemHandle(), WM_SETICON, ICON_SMALL, (LPARAM)icon);
    SendMessage(window.getSystemHandle(), WM_SETICON, ICON_BIG, (LPARAM)icon);
    #elif __APPLE__
    #else
    {
      sf::Image icon = resources.gfx->texture("icon").copyToImage();
      window.setIcon(128, 128, icon.getPixelsPtr());
    }
    #endif

    UI gui(window, game);

    resources.gfx->tGui.setView(view);

    GameDraw gameDraw(resources, *gui.guiElements, game.drawMe);

    game.rander.seedPiece(time(NULL)); // Make sure the seed is random-ish in case the client never connects
    game.rander.seedHole(time(NULL));

    // Intro

    {
      bool intro=true;
      sf::Sprite logo = resources.gfx->sprite("logo");
      resources.delayClock.restart();
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
          float timing = resources.delayClock.getElapsedTime().asMilliseconds() / 100.0;
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
          logo.setPosition(posX, posY);
          logo.setScale(scale, scale);
          gui.guiElements->animatedBackground.draw(window, gui.delayClock.getElapsedTime());
          window.draw(logo);
          window.display();

          sf::sleep(sf::milliseconds(20));
      }
    }

    // The main-loop

    gameDraw.init();

    window.setActive(false);

    while (window.isOpen())
    {
        sf::Event event;

        while (window.pollEvent(event))
            if (gui.handleEvent(event))
                game.state->handleEvent(event);

        if (resources.playonline)
            while (resources.net->receiveData()) {}

        gui.delayCheck();
        game.state->update();
        gameDraw.draw();
    }

    // Things to do before the game turns off

    game.field.status=5;
    gameDraw.quit();
    if (game.field.drawThread.joinable())
        game.field.drawThread.join();

    Options::save();
    if (resources.restart) {
        #ifdef _WIN32
            system("start SpeedBlocks.exe");
        #elif __APPLE__
            std::string cmd = "open -n '" + resourcePath() + "../../../SpeedBlocks.app'";
            system(cmd.c_str());
        #else
            system("./SpeedBlocks");
        #endif
    }

    return 0;
}
