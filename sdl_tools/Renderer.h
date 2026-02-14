//
// Created by user on 14/02/2026.
//

#ifndef GAME_RENDERER_H
#define GAME_RENDERER_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "Window.h"
#include "Error.h"

using namespace std;

class Renderer {
    public:
    Renderer(const Window& w, int index, int flags, Error& err);
    ~Renderer();
    SDL_Renderer* renderer;
};


#endif //GAME_RENDERER_H