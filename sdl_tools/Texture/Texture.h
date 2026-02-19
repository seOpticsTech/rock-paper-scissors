//
// Created by user on 14/02/2026.
//

#ifndef GAME_TEXTURE_H
#define GAME_TEXTURE_H
#include <SDL_render.h>
#include "Error.h"
#include "CyclicList/CyclicList.h"


class Texture {
    public:
    Texture(SDL_Texture* texture);
    Texture(SDL_Texture* texture, const SDL_Rect& scope);
    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;
    Texture(Texture&& other) noexcept;
    Texture& operator=(Texture&& other) noexcept;
    ~Texture();
    void querySize(int& width, int& height, Error& err) const;
    void setScope(const SDL_Rect& rect);
    void setFullScope();
    const SDL_Rect* getScope() const;
    SDL_Texture* texture;
    protected:
    SDL_Rect* scope;
};

typedef CyclicList<Texture*>::iterator Animation;


#endif //GAME_TEXTURE_H
