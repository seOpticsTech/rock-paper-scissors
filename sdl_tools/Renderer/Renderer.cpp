//
// Created by user on 14/02/2026.
//

#include "Renderer.h"
#include "Vector/Vector.h"
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

Texture* Renderer::loadTexture(const string &filePath, Error &err) const {
    SDL_Texture* t = IMG_LoadTexture(this->renderer, filePath.c_str());
    if (t == nullptr) {
        err = Error::New(IMG_GetError());
        return nullptr;
    }

    return new Texture(t);
}

Texture* Renderer::loadTexture(const string &filePath, const SDL_Rect& scope, Error &err) const {
    SDL_Texture* t = IMG_LoadTexture(this->renderer, filePath.c_str());
    if (t == nullptr) {
        err = Error::New(IMG_GetError());
        return nullptr;
    }

    return new Texture{t, scope};
}

void Renderer::setDrawColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a) const {
    SDL_SetRenderDrawColor(renderer, r, g, b, a);
}

void Renderer::clear() const {
    SDL_RenderClear(renderer);
}

void Renderer::copy(Texture *texture, const Vector& position, Error& err) const {
    SDL_Rect dst;
    texture->querySize(dst.w, dst.h, err);
    if (err.status == failure) {
        return;
    }
    dst.x = static_cast<int>(position[0]);
    dst.y = static_cast<int>(position[1]);
    SDL_RenderCopy(renderer, texture->texture, texture->getScope(), &dst);
}

void Renderer::present() const {
    SDL_RenderPresent(renderer);
}
