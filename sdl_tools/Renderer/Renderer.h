//
// Created by user on 14/02/2026.
//

#ifndef GAME_RENDERER_H
#define GAME_RENDERER_H

#include <SDL2/SDL.h>
#include "Window/Window.h"
#include "Error.h"
#include "Texture/Texture.h"
#include "Vector/Vector.h"
#include <initializer_list>
#include <utility>

using namespace std;

class Renderer {
    public:
    Renderer(const Window& w, int index, int flags, Error& err);
    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;
    Renderer(Renderer&& other) noexcept;
    Renderer& operator=(Renderer&& other) noexcept;
    ~Renderer();
    Texture* loadTexture(const string& filePath, Error& err) const;
    Texture* loadTexture(const string &filePath, const SDL_Rect& scope, Error &err) const;
    Animation loadAnimation(std::initializer_list<string> paths, Error& err) const;
    Animation loadAnimation(std::initializer_list<pair<string, const SDL_Rect*>> pathesScopes, Error& err) const;
    void setDrawColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a) const;
    void clear() const;
    void copy(Texture* texture, const Vector& position, Error& err) const;
    void present() const;
    SDL_Renderer* renderer;
};


#endif //GAME_RENDERER_H
