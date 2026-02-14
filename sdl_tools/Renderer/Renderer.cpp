//
// Created by user on 14/02/2026.
//

#include "Renderer.h"
#include <SDL.h>
#include <SDL_image.h>


Renderer::Renderer(const Window &w, int index, int flags, Error& err) {
    renderer = SDL_CreateRenderer(w.window, index, flags);
    if (renderer == nullptr) {
        err = Error::New(SDL_GetError());
    }
}

Renderer::Renderer(Renderer&& other) noexcept : renderer(other.renderer) {
    other.renderer = nullptr;
}

Renderer& Renderer::operator=(Renderer&& other) noexcept {
    if (this != &other) {
        SDL_DestroyRenderer(renderer);
        renderer = other.renderer;
        other.renderer = nullptr;
    }
    return *this;
}

Renderer::~Renderer() {
    SDL_DestroyRenderer(renderer);
}

Texture Renderer::loadTexture(const string &filePath, Error &err) const {
    SDL_Texture* t = IMG_LoadTexture(this->renderer, filePath.c_str());
    if (t == nullptr) {
        err = Error::New(IMG_GetError());
        return t;
    }

    return t;
}

void Renderer::setDrawColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a) const {
    SDL_SetRenderDrawColor(renderer, r, g, b, a);
}

void Renderer::clear() const {
    SDL_RenderClear(renderer);
}

void Renderer::copy(const Texture &texture, const SDL_Rect *srcRect, const SDL_Rect *dstRect) const {
    SDL_RenderCopy(renderer, texture.texture, srcRect, dstRect);
}

void Renderer::present() const {
    SDL_RenderPresent(renderer);
}
