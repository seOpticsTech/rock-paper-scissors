//
// Created by user on 14/02/2026.
//

#include "SDL_Environment.h"
#include <string>

using namespace std;

SDL_Environment::SDL_Environment(int windowWidth, int windowHeight, Error& err) : window(nullptr), renderer(nullptr) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        err = Error(SDL_GetError());
        return;
    }
    window = new Window(
    string("game"),
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        windowWidth,
        windowHeight,
    SDL_WINDOW_SHOWN,
    err
    );
    if (err.status == failure) {
        err = Error(string("Failed to create window: ") + err.message);
        cleanup();
        return;
    }

    renderer = new Renderer(
        *window,
   -1,
    SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC,
        err
    );
    if (err.status == failure) {
        err = Error(string("Failed to create renderer: ") + err.message);
        cleanup();
        return;
    }

    if (!IMG_Init(IMG_INIT_PNG)) {
        err = Error(SDL_GetError());
        cleanup();
        return;
    }
}

SDL_Environment::~SDL_Environment() {
    IMG_Quit();
    cleanup();
}

void SDL_Environment::cleanup() const {
    delete renderer;
    delete window;
    SDL_Quit();
}
