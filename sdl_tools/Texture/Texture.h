//
// Created by user on 14/02/2026.
//

#ifndef GAME_TEXTURE_H
#define GAME_TEXTURE_H
#include <SDL_render.h>
#include "Error.h"


class Texture {
    public:
    Texture(SDL_Texture* texture);
    ~Texture();
    void querySize(int& width, int& height, Error& err) const;
    SDL_Texture* texture;
};


#endif //GAME_TEXTURE_H
