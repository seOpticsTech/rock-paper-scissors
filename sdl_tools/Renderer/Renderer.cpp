//
// Created by user on 14/02/2026.
//

#include "Renderer.h"
#include "Vector/Vector.h"
#include <SDL.h>
#include <SDL_image.h>
#include <list>
#include <vector>


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

Animation Renderer::loadAnimation(std::initializer_list<string> paths, int msPerFrame, Error& err) const {
    if (paths.size() == 0) {
        err = Error::New("Animation requires at least one frame");
        return {};
    }

    list<Texture*> frames;
    for (const string& path : paths) {
        Texture* next = loadTexture(path, err);
        if (err.status == failure) {
            break;
        }
        frames.push_back(next);
    }

    if (err.status == failure) {
        for (Texture* frame : frames) {
            delete frame;
        }
        return {};
    }

    auto animation = new CyclicList<Texture*>();
    for (Texture* frame : frames) {
        animation->push_back(frame);
    }
    return Animation(animation->cyclic_begin(), msPerFrame);
}

Animation Renderer::loadAnimation(std::initializer_list<pair<string, const SDL_Rect*>> pathesScopes, int msPerFrame, Error& err) const {
    if (pathesScopes.size() == 0) {
        err = Error::New("Animation requires at least one frame");
        return {};
    }

    list<Texture*> frames;
    for (const auto& pathScope : pathesScopes) {
        Texture* next = nullptr;
        const string& path = pathScope.first;
        const SDL_Rect* scope = pathScope.second;
        if (scope == nullptr) {
            next = loadTexture(path, err);
        } else {
            next = loadTexture(path, *scope, err);
        }
        if (err.status == failure) {
            break;
        }
        frames.push_back(next);
    }

    if (err.status == failure) {
        for (Texture* frame : frames) {
            delete frame;
        }
        return {};
    }

    auto animation = new CyclicList<Texture*>();
    for (Texture* frame : frames) {
        animation->push_back(frame);
    }
    return Animation(animation->cyclic_begin(), msPerFrame);
}

void Renderer::setDrawColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a) const {
    SDL_SetRenderDrawColor(renderer, r, g, b, a);
}

void Renderer::clear() const {
    SDL_RenderClear(renderer);
}

void Renderer::copy(Texture *texture, const Vector& position, Error& err) const {
    if (texture == nullptr) {
        err = Error::New("Texture is null");
        return;
    }
    SDL_Rect dst;
    const Vector* drawSize = texture->getDrawSize();
    if (drawSize == nullptr) {
        texture->querySize(dst.w, dst.h, err);
        if (err.status == failure) {
            return;
        }
    } else {
        dst.w = static_cast<int>((*drawSize)[0]);
        dst.h = static_cast<int>((*drawSize)[1]);
    }
    dst.x = static_cast<int>(position[0]);
    dst.y = static_cast<int>(position[1]);
    if (SDL_RenderCopy(renderer, texture->texture, texture->getScope(), &dst)) {
        err = Error::New(string("Error in SDL_RenderCopy: ") + SDL_GetError());
    }
}

void Renderer::present() const {
    SDL_RenderPresent(renderer);
}
