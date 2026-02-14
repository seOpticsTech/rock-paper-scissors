//
// Created by user on 14/02/2026.
//

#include "Texture.h"


Texture::Texture(SDL_Texture* texture) : texture(texture) {}

Texture::~Texture() {
    SDL_DestroyTexture(texture);
}

void Texture::querySize(int &width, int &height, Error &err) const {
    if (SDL_QueryTexture(texture, nullptr, nullptr, &width, &height) != 0) {
        err = Error::New(SDL_GetError());
    }
}
