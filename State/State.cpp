//
// Created by user on 14/02/2026.
//

#include "State.h"
#include "Actor/Actors/View/View.h"
#include <map>
#include <iostream>
#include <unordered_set>

using namespace std;

typedef Error (*eventHandler)(const SDL_Event&);

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

Error handleQuitEvent(const SDL_Event& event) {
    static_cast<void>(event);
    State& state = State::get();
    for (pair<string, Actor*> nameActor : state.actors) {
        Actor* actor = nameActor.second;
        if (actor == nullptr) {
            continue;
        }
        auto groupIt = actor->eventActions.find(Actor::quit);
        if (groupIt == actor->eventActions.end()) {
            continue;
        }
        Uint32 key = 0;
        auto actionIt = groupIt->second.find(key);
        if (actionIt == groupIt->second.end() || actionIt->second == nullptr) {
            continue;
        }
        actionIt->second(*actor, event);
    }
    state.running = false;
    return Error::Success();
}

Error handleKeyDownEvent(const SDL_Event& event) {
    State& state = State::get();
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

Error handleKeyUpEvent(const SDL_Event& event) {
    State& state = State::get();
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
    : textures(), env(nullptr), running(true), actors(), textureCleanupEveryXFrames(1000), view(nullptr) {
    env = new SDL_Environment(config, err);
    if (err.status == failure) {
        err = Error::New("Failed to set up SDL_Environment: " + err.message);
        return;
    }

    view = new View(800, 600);
    view->actor = addActor("view", err);
    if (err.status == failure) {
        err = Error::New("Failed to set up View: " + err.message);
        return;
    }
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

    SDL_Event event;
    map<Uint32, eventHandler> typeToHandler;
    typeToHandler[SDL_QUIT] = handleQuitEvent;
    typeToHandler[SDL_KEYDOWN] = handleKeyDownEvent;
    typeToHandler[SDL_KEYUP] = handleKeyUpEvent;

    int frameCounter = 0;

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
            err = handle(event);
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
            if (actor->onRender != nullptr) {
                actor->onRender(*actor);
            }

            string cur = actor->currentTexture;
            if (!cur.empty()) {
                auto texIt = actor->textures.find(cur);
                if (texIt == actor->textures.end()) {
                    continue;
                }
                const string& textureName = texIt->second;
                auto stateTexIt = textures.find(textureName);
                if (stateTexIt == textures.end() || stateTexIt->second == nullptr) {
                    cerr << "Missing texture: " << textureName << endl;
                    continue;
                }
                env->renderer->copy(stateTexIt->second, actor->position - view->actor->position, err);
                if (err.status == failure) {
                    cerr << "Texture copy error: " << err.message << endl;
                }
            }
        }

        env->renderer->present();
        if (frameCounter % textureCleanupEveryXFrames == 0) {
            cleanupTextures();
        }
        frameCounter++;
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
        err = Error::New(string("Texture already exists: ") + name, Error::duplicate);
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

void State::cleanupTextures() {
    unordered_set<string> activeTextures;
    for (pair<string, Actor*> nameActor : actors) {
        Actor* actor = nameActor.second;
        for (pair<string, string> textureNames : actor->textures) {
            string textureName = textureNames.second;
            activeTextures.insert(textureName);
        }
    }

    for (pair<string, Texture*> nameTexture : textures) {
        string name = nameTexture.first;
        Texture* texture = nameTexture.second;
        if (!activeTextures.contains(name)) {
            textures.erase(name);
            delete texture;
        }
    }
}