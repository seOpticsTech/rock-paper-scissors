//
// Created by user on 14/02/2026.
//

#ifndef GAME_SDL_ENVIRONMENT_H
#define GAME_SDL_ENVIRONMENT_H
#include "Renderer.h"
#include "Window.h"
#include "Error.h"


class SDL_Environment {
    public:
    SDL_Environment(int windowWidth, int windowHeight, Error& err);
    ~SDL_Environment();

    Window* window;
    Renderer* renderer;
    private:
    void cleanup() const;

};


#endif //GAME_SDL_ENVIRONMENT_H