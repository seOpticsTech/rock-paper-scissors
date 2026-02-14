//
// Created by user on 14/02/2026.
//

#include "SDL_Environment.h"
#include <string>
#include <SDL_image.h>

using namespace std;

SDL_Environment::SDL_Environment(const Config& config, Error& err)
    : window(nullptr),
      renderer(nullptr),
      sdlInitialized(false),
      imgInitialized(false) {
    if (SDL_Init(config.sdlInitFlags) != 0) {
        err = Error(SDL_GetError());
        return;
    }
    sdlInitialized = true;
    window = new Window(
        config.windowTitle,
        config.windowX,
        config.windowY,
        config.windowWidth,
        config.windowHeight,
        config.windowFlags,
        err
    );
    if (err.status == failure) {
        err = Error(string("Failed to create window: ") + err.message);
        cleanup();
        return;
    }

    renderer = new Renderer(
        *window,
        config.rendererIndex,
        config.rendererFlags,
        err
    );
    if (err.status == failure) {
        err = Error(string("Failed to create renderer: ") + err.message);
        cleanup();
        return;
    }

    if ((IMG_Init(config.imgInitFlags) & config.imgInitFlags) != config.imgInitFlags) {
        err = Error(IMG_GetError());
        cleanup();
        return;
    }
    imgInitialized = true;
}

SDL_Environment::SDL_Environment(SDL_Environment&& other) noexcept
    : window(other.window),
      renderer(other.renderer),
      sdlInitialized(other.sdlInitialized),
      imgInitialized(other.imgInitialized) {
    other.window = nullptr;
    other.renderer = nullptr;
    other.sdlInitialized = false;
    other.imgInitialized = false;
}

SDL_Environment& SDL_Environment::operator=(SDL_Environment&& other) noexcept {
    if (this != &other) {
        if (imgInitialized) {
            IMG_Quit();
            imgInitialized = false;
        }
        cleanup();
        window = other.window;
        renderer = other.renderer;
        sdlInitialized = other.sdlInitialized;
        imgInitialized = other.imgInitialized;
        other.window = nullptr;
        other.renderer = nullptr;
        other.sdlInitialized = false;
        other.imgInitialized = false;
    }
    return *this;
}

SDL_Environment::~SDL_Environment() {
    cleanup();
}

void SDL_Environment::cleanup() {
    if (imgInitialized) {
        IMG_Quit();
        imgInitialized = false;
    }
    delete renderer;
    renderer = nullptr;
    delete window;
    window = nullptr;
    if (sdlInitialized) {
        SDL_Quit();
        sdlInitialized = false;
    }
}
