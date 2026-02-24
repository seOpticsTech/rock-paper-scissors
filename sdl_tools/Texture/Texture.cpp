//
// Created by user on 14/02/2026.
//

#include "Texture.h"


Texture::Texture(SDL_Texture* texture) : texture(texture), scope(nullptr), drawSize(nullptr) {}

Texture::Texture(SDL_Texture* texture, const SDL_Rect& scope)
    : texture(texture), scope(new SDL_Rect{scope.x, scope.y, scope.w, scope.h}), drawSize(nullptr) {}

Texture::Texture(Texture&& other) noexcept
    : texture(other.texture), scope(other.scope), drawSize(other.drawSize) {
    other.texture = nullptr;
    other.scope = nullptr;
    other.drawSize = nullptr;
}

Texture& Texture::operator=(Texture&& other) noexcept {
    if (this != &other) {
        SDL_DestroyTexture(texture);
        delete scope;
        texture = other.texture;
        scope = other.scope;
        drawSize = other.drawSize;
        other.texture = nullptr;
        other.scope = nullptr;
        other.drawSize = nullptr;
    }
    return *this;
}

Texture::~Texture() {
    SDL_DestroyTexture(texture);
    delete scope;
    delete drawSize;
}

void Texture::setScope(const SDL_Rect& rect) {
    if (scope == nullptr) {
        scope = new SDL_Rect{rect.x, rect.y, rect.w, rect.h};
        return;
    }
    scope->x = rect.x;
    scope->y = rect.y;
    scope->w = rect.w;
    scope->h = rect.h;
}

void Texture::setFullScope() {
    delete scope;
    scope = nullptr;
}

const SDL_Rect* Texture::getScope() const {
    return scope;
}

void Texture::setDrawSize(double width, double height) {
    if (drawSize == nullptr) {
        drawSize = new Vector(width, height);
        return;
    }
    (*drawSize)[0] = width;
    (*drawSize)[1] = height;
}

void Texture::clearDrawSize() {
    delete drawSize;
    drawSize = nullptr;
}

const Vector* Texture::getDrawSize() const {
    return drawSize;
}


void Texture::querySize(int &width, int &height, Error &err) const {
    if (texture == nullptr) {
        err = Error::New("SDL_Texture is null");
        return;
    }

    if (SDL_QueryTexture(texture, nullptr, nullptr, &width, &height) != 0) {
        err = Error::New(SDL_GetError());
    }
}
