//
// Created by user on 14/02/2026.
//

#ifndef GAME_CONFIG_H
#define GAME_CONFIG_H

#include <SDL2/SDL.h>
#include <functional>
#include <string>

using namespace std;

struct Config {
    Uint32 sdlInitFlags;
    string windowTitle;
    int windowX;
    int windowY;
    int windowWidth;
    int windowHeight;
    Uint32 windowFlags;
    int rendererIndex;
    Uint32 rendererFlags;
    int imgInitFlags;
    function<int(SDL_Event*)> pollEvent;
};

Config getDefaultConfig();

#endif //GAME_CONFIG_H
