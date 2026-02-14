//
// Created by user on 14/02/2026.
//

#ifndef GAME_RENDERER_H
#define GAME_RENDERER_H

#include <SDL2/SDL.h>
#include "Window/Window.h"
#include "Error.h"
#include "Texture/Texture.h"

using namespace std;

class Renderer {
    public:
    Renderer(const Window& w, int index, int flags, Error& err);
    ~Renderer();
    Texture loadTexture(const string& filePath, Error& err) const;
    void setDrawColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a) const;
    void clear() const;
    void copy(const Texture& texture, const SDL_Rect* srcRect, const SDL_Rect* dstRect) const;
    void present() const;
    SDL_Renderer* renderer;
};


#endif //GAME_RENDERER_H
