//
// Created by user on 14/02/2026.
//

#ifndef GAME_WINDOW_H
#define GAME_WINDOW_H

#include <SDL_stdinc.h>
#include <SDL2/SDL.h>
#include <string>
#include "Error.h"
using namespace std;

class Window {
    public:
    Window(const string& title, int x, int y, int w, int h, Uint32 flags, Error& err);
    ~Window();
    SDL_Window* window;
};


#endif //GAME_WINDOW_H
