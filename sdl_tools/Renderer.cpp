//
// Created by user on 14/02/2026.
//

#include "Renderer.h"
#include <SDL.h>


Renderer::Renderer(const Window &w, int index, int flags, Error& err) {
    renderer = SDL_CreateRenderer(w.window, index, flags);
    if (renderer == nullptr) {
        err = Error::New(SDL_GetError());
    }
}

Renderer::~Renderer() {
    SDL_DestroyRenderer(renderer);
}
