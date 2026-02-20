//
// Created by user on 14/02/2026.
//

#include "Config.h"
#include <SDL2/SDL_image.h>

Config getDefaultConfig() {
    Config config{};
    config.sdlInitFlags = SDL_INIT_VIDEO;
    config.windowTitle = "game";
    config.windowX = SDL_WINDOWPOS_CENTERED;
    config.windowY = SDL_WINDOWPOS_CENTERED;
    config.windowWidth = 800;
    config.windowHeight = 600;
    config.windowFlags = SDL_WINDOW_SHOWN;
    config.rendererIndex = -1;
    config.rendererFlags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;
    config.imgInitFlags = IMG_INIT_PNG;
    config.pollEvent = SDL_PollEvent;
    return config;
}
