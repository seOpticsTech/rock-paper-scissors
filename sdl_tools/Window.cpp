//
// Created by user on 14/02/2026.
//

#include "Window.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <string>
#include "Error.h"

using namespace std;

Window::Window(const string& title, int x, int y, int w, int h, Uint32 flags, Error& err) {
    window = SDL_CreateWindow(title.c_str(), x, y, w, h, flags);
    if (window == nullptr) {
        err = Error::New(SDL_GetError());
    }
}

Window::~Window() {
    SDL_DestroyWindow(window);
}
