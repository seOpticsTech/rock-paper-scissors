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
    actors["player"]->textures["main"]->querySize(texWidth, texHeight, err);
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
        err = Error::Success();

        env->renderer->setDrawColor(0, 0, 0, 255);
        env->renderer->clear();

        for (pair<string, Actor*> nameActor : actors) {
            Actor* actor = nameActor.second;

            actor->position = actor->position + actor->velocity;
            actor->velocity = actor->velocity + actor->acceleration;

            string cur = actor->currentTexture;
            Texture* t = actor->textures[cur];
            if (t == nullptr) {
                continue;
            }
            env->renderer->copy(t, actor->position, err);
            if (err.status == failure) {
                cerr << "Texture copy error: " << err.message << endl;
            }
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

Texture* State::loadTexture(const string& name, const string &filePath, const SDL_Rect& scope, Error& err) {
    Texture* t = env->renderer->loadTexture(filePath, scope, err);
    if (err.status == failure) {
        return nullptr;
    }
    textures[name] = t;
    return t;
}
