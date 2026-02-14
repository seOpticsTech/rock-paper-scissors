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
    : env(new SDL_Environment(config, err)), running(true), actors() {}

State::State(State&& other) noexcept
    : env(other.env), running(other.running), actors(move(other.actors)) {
    other.env = nullptr;
    other.running = false;
}

State& State::operator=(State&& other) noexcept {
    if (this != &other) {
        for (Actor* actor : actors) {
            delete actor;
        }
        actors.clear();
        delete env;
        env = other.env;
        running = other.running;
        actors = move(other.actors);
        other.env = nullptr;
        other.running = false;
    }
    return *this;
}

State::~State() {
    for (Actor* actor : actors) {
        delete actor;
    }
    actors.clear();
    delete env;
}

void State::startEventLoop() {
    if (env == nullptr) {
        return;
    }

    Error err;
    const auto playerTexture = env->renderer->loadTexture("assets/player.png", err);
    if (err.status == failure) {
        cerr << "SDL_Environment load error: " << err.message << endl;
        return;
    }

    int texWidth = 0;
    int texHeight = 0;
    playerTexture.querySize(texWidth, texHeight, err);
    if (err.status == failure) {
        cerr << "Texture size error: " << err.message << endl;
        return;
    }

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

        for (Actor* actor : actors) {

            SDL_Rect dstRect;
            dstRect.w = texWidth;
            dstRect.h = texHeight;
            dstRect.x = (800 - texWidth) / 2;
            dstRect.y = (600 - texHeight) / 2;
            string cur = actor->currentTexture;
            Texture t = actor->textures[cur];
            env->renderer->copy(actor->textures[actor->currentTexture], &dstRect);

        }

        env->renderer->present();
    }
}
