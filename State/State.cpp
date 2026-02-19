//
// Created by user on 14/02/2026.
//

#include "State.h"
#include <map>
#include <iostream>

using namespace std;

typedef Error (*eventHandler)(State&);

Error handleQuitEvent(State& state) {
    state.running = false;
    return Error::Success();
}

State::State(const Config& config, Error& err)
    : textures(), env(new SDL_Environment(config, err)), running(true), actors() {}

State::State(State&& other) noexcept
    : textures(move(other.textures)), env(other.env), running(other.running), actors(move(other.actors)) {
    other.env = nullptr;
    other.running = false;
}

State& State::operator=(State&& other) noexcept {
    if (this != &other) {
        for (pair<string, Actor*> actor : actors) {
            delete actor.second;
        }
        for (pair<string, Texture*> texture : textures) {
            delete texture.second;
        }
        actors.clear();
        textures.clear();
        delete env;
        env = other.env;
        running = other.running;
        actors = move(other.actors);
        textures = move(other.textures);
        other.env = nullptr;
        other.running = false;
    }
    return *this;
}

State::~State() {
    for (pair<string, Actor*> actor : actors) {
        delete actor.second;
    }
    for (pair<string, Texture*> texture : textures) {
        delete texture.second;
    }
    actors.clear();
    delete env;
}

void State::startEventLoop() {
    if (env == nullptr) {
        return;
    }

    Error err;

    int texWidth = 0;
    int texHeight = 0;

    SDL_Event event;
    map<Uint32, eventHandler> typeToHandler;
    typeToHandler[SDL_QUIT] = handleQuitEvent;

    while (running) {
        while (SDL_PollEvent(&event)) {
            eventHandler handle = typeToHandler[event.type];
            if (handle == nullptr) {
                continue;
            }
            err = handle(*this);
            if (err.status == failure) {
                cerr << "SDL_Event type error: " << err.message << endl;
            }
        }

        env->renderer->setDrawColor(0, 0, 0, 255);
        env->renderer->clear();

        for (pair<string, Actor*> nameActor : actors) {
            Actor* actor = nameActor.second;
            SDL_Rect dstRect;
            dstRect.w = texWidth;
            dstRect.h = texHeight;
            dstRect.x = (800 - texWidth) / 2;
            dstRect.y = (600 - texHeight) / 2;
            string cur = actor->currentTexture;
            const Texture& t = *(actor->textures[cur]);
            env->renderer->copy(t, &dstRect);
        }

        env->renderer->present();
    }
}

Actor* State::addActor(const string& name) {
    auto a = new Actor();
    actors[name] = a;
    return a;
}

Texture* State::loadTexture(const string& name, const string &filePath, Error& err) {
    Texture* t = env->renderer->loadTexture(filePath, err);
    if (err.status == failure) {
        return nullptr;
    }
    textures[name] = t;
    return t;
}
