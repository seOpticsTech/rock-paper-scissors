//
// Created by user on 14/02/2026.
//

#ifndef GAME_SDL_ENVIRONMENT_H
#define GAME_SDL_ENVIRONMENT_H
#include "Config/Config.h"
#include "Renderer/Renderer.h"
#include "Window/Window.h"
#include "Error.h"


class SDL_Environment {
    public:
    SDL_Environment(const Config& config, Error& err);
    SDL_Environment(const SDL_Environment&) = delete;
    SDL_Environment& operator=(const SDL_Environment&) = delete;
    SDL_Environment(SDL_Environment&& other) noexcept;
    SDL_Environment& operator=(SDL_Environment&& other) noexcept;
    ~SDL_Environment();

    Window* window;
    Renderer* renderer;
    protected:
    void cleanup();
    bool sdlInitialized;
    bool imgInitialized;

};


#endif //GAME_SDL_ENVIRONMENT_H
