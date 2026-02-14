//
// Created by user on 14/02/2026.
//

#include "Texture.h"


Texture::Texture(SDL_Texture* texture) : texture(texture) {}

Texture::Texture(Texture&& other) noexcept : texture(other.texture) {
    other.texture = nullptr;
}

Texture& Texture::operator=(Texture&& other) noexcept {
    if (this != &other) {
        SDL_DestroyTexture(texture);
        texture = other.texture;
        other.texture = nullptr;
    }
    return *this;
}

Texture::~Texture() {
    SDL_DestroyTexture(texture);
}

void Texture::querySize(int &width, int &height, Error &err) const {
    if (texture == nullptr) {
        err = Error::New("Texture is null");
        return;
    }

    if (SDL_QueryTexture(texture, nullptr, nullptr, &width, &height) != 0) {
        err = Error::New(SDL_GetError());
        return;
    }
}
