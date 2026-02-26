//
// Created by user on 14/02/2026.
//

#include "Config.h"
#include <SDL2/SDL_image.h>

Config getDefaultConfig() {
    Config config{};
    config.sdlInitFlags = SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER;
    config.windowTitle = "game";
    config.windowX = SDL_WINDOWPOS_CENTERED;
    config.windowY = SDL_WINDOWPOS_CENTERED;
    config.windowWidth = 1920;
    config.windowHeight = 1080;
    config.windowFlags = SDL_WINDOW_SHOWN;
    config.rendererIndex = -1;
    config.rendererFlags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;
    config.imgInitFlags = IMG_INIT_PNG;
    config.fps = 60;
    config.pollEvent = SDL_PollEvent;
    config.controlMode = KEYBOARD;
    config.localPlayer = "player_1";
    config.toConnect = false;
    config.connectionDetails.remoteHost = "127.0.0.1";
    config.connectionDetails.localPort = 4000;
    config.connectionDetails.remotePort = 4001;
    return config;
}
