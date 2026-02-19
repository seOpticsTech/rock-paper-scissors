//
// Created by user on 14/02/2026.
//

#include "State.h"
#include <map>
#include <iostream>

using namespace std;

typedef Error (*eventHandler)(State&, const SDL_Event&);

State* State::instance = nullptr;

void State::make(const Config& config, Error& err) {
    if (instance != nullptr) {
        err = Error::New("Instance is already created.");
        return;
    }
    instance = new State(config, err);
    if (err.status == failure) {
        cerr << "Couldn't create initial state: " << err.message << endl;
        delete instance;
        instance = nullptr;
    }
}

void State::destroy() {
    delete instance;
    instance = nullptr;
}

State& State::get() {
    return *instance;
}

Error handleQuitEvent(State& state, const SDL_Event& event) {
    static_cast<void>(event);
    state.running = false;
    return Error::Success();
}

Error handleKeyDownEvent(State& state, const SDL_Event& event) {
    for (pair<string, Actor*> nameActor : state.actors) {
        Actor* actor = nameActor.second;
        if (actor == nullptr) {
            continue;
        }
        auto groupIt = actor->eventActions.find(Actor::keyDown);
        if (groupIt == actor->eventActions.end()) {
            continue;
        }
        Uint32 key = static_cast<Uint32>(event.key.keysym.sym);
        auto actionIt = groupIt->second.find(key);
        if (actionIt == groupIt->second.end() || actionIt->second == nullptr) {
            continue;
        }
        actionIt->second(*actor, event);
    }
    return Error::Success();
}

Error handleKeyUpEvent(State& state, const SDL_Event& event) {
    for (pair<string, Actor*> nameActor : state.actors) {
        Actor* actor = nameActor.second;
        if (actor == nullptr) {
            continue;
        }
        auto groupIt = actor->eventActions.find(Actor::keyUp);
        if (groupIt == actor->eventActions.end()) {
            continue;
        }
        Uint32 key = static_cast<Uint32>(event.key.keysym.sym);
        auto actionIt = groupIt->second.find(key);
        if (actionIt == groupIt->second.end() || actionIt->second == nullptr) {
            continue;
        }
        actionIt->second(*actor, event);
    }
    return Error::Success();
}

State::State(const Config& config, Error& err)
    : textures(), env(new SDL_Environment(config, err)), running(true), actors() {}

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

    SDL_Event event;
    map<Uint32, eventHandler> typeToHandler;
    typeToHandler[SDL_QUIT] = handleQuitEvent;
    typeToHandler[SDL_KEYDOWN] = handleKeyDownEvent;
    typeToHandler[SDL_KEYUP] = handleKeyUpEvent;

    while (running) {
        while (SDL_PollEvent(&event)) {
            auto handlerIt = typeToHandler.find(event.type);
            if (handlerIt == typeToHandler.end()) {
                continue;
            }
            eventHandler handle = handlerIt->second;
            if (handle == nullptr) {
                continue;
            }
            err = handle(*this, event);
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
            auto texIt = actor->textures.find(cur);
            if (texIt == actor->textures.end() || texIt->second == nullptr) {
                continue;
            }
            env->renderer->copy(texIt->second, actor->position, err);
            if (err.status == failure) {
                cerr << "Texture copy error: " << err.message << endl;
            }
        }

        env->renderer->present();
    }
}

Actor* State::addActor(const string& name, Error& err) {
    if (actors.contains(name)) {
        err = Error::New(string("Actor with name ") + name + " already exists");
        return nullptr;
    }
    auto a = new Actor();
    actors[name] = a;
    return a;
}

Texture* State::loadTexture(const string& name, const string &filePath, Error& err) {
    if (textures.find(name) != textures.end()) {
        err = Error::New(string("Texture already exists: ") + name);
        return nullptr;
    }
    Texture* t = env->renderer->loadTexture(filePath, err);
    if (err.status == failure) {
        return nullptr;
    }
    textures[name] = t;
    return t;
}

Texture* State::loadTexture(const string& name, const string &filePath, const SDL_Rect& scope, Error& err) {
    if (textures.find(name) != textures.end()) {
        err = Error::New(string("Texture already exists: ") + name);
        return nullptr;
    }
    Texture* t = env->renderer->loadTexture(filePath, scope, err);
    if (err.status == failure) {
        return nullptr;
    }
    textures[name] = t;
    return t;
}
