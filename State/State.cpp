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
    : animations(), env(nullptr), running(true), actors(), animationCleanupEveryXFrames(1000), view(nullptr) {
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
    for (pair<string, Animation> nameAnimation : animations) {
        Animation& anim = nameAnimation.second;
        auto list = !anim.iterator;
        if (list == nullptr) {
            continue;
        }
        for (auto it = list->begin(); it != list->end(); ++it) {
            delete *it;
        }
        delete list;
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

            string cur = actor->currentAnimation;
            if (!cur.empty()) {
                auto animNameIt = actor->animations.find(cur);
                if (animNameIt == actor->animations.end()) {
                    continue;
                }
                const string& animationName = animNameIt->second;
                auto animIt = animations.find(animationName);
                if (animIt == animations.end()) {
                    cerr << "Missing animation: " << animationName << endl;
                    continue;
                }
                Animation& anim = animIt->second;
                auto list = !anim.iterator;
                if (list == nullptr || list->empty()) {
                    cerr << "Missing animation frames: " << animationName << endl;
                    continue;
                }
                Texture* frame = *anim.iterator;
                if (frame == nullptr) {
                    cerr << "Missing animation frame: " << animationName << endl;
                    continue;
                }
                env->renderer->copy(frame, actor->position - view->actor->position, err);
                if (err.status == failure) {
                    cerr << "Texture copy error: " << err.message << endl;
                }
                ++anim.iterator;
            }
        }

        env->renderer->present();
        if (frameCounter % animationCleanupEveryXFrames == 0) {
            cleanupAnimations();
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

Animation State::loadAnimation(const string& name, std::initializer_list<string> paths, Error& err) {
    if (animations.find(name) != animations.end()) {
        err = Error::New(string("Animation already exists: ") + name, Error::duplicate);
        return {};
    }
    Animation anim = env->renderer->loadAnimation(paths, err);
    if (err.status == failure) {
        auto list = !anim.iterator;
        if (list != nullptr) {
            for (auto it = list->begin(); it != list->end(); ++it) {
                delete *it;
            }
            delete list;
        }
        return {};
    }
    animations[name] = anim;
    return anim;
}

Animation State::loadAnimation(const string& name, std::initializer_list<pair<string, const SDL_Rect*>> frames, Error& err) {
    if (animations.find(name) != animations.end()) {
        err = Error::New(string("Animation already exists: ") + name, Error::duplicate);
        return {};
    }
    Animation anim = env->renderer->loadAnimation(frames, err);
    if (err.status == failure) {
        auto list = !anim.iterator;
        if (list != nullptr) {
            for (auto it = list->begin(); it != list->end(); ++it) {
                delete *it;
            }
            delete list;
        }
        return {};
    }
    animations[name] = anim;
    return anim;
}

void State::cleanupAnimations() {
    unordered_set<string> activeAnimations;
    for (pair<string, Actor*> nameActor : actors) {
        Actor* actor = nameActor.second;
        for (pair<string, string> animationPair : actor->animations) {
            string animationName = animationPair.second;
            activeAnimations.insert(animationName);
        }
    }

    unordered_set<string> keysToDelete;
    for (const pair<const string, Animation>& animationName : animations) {
        const string& name = animationName.first;
        const Animation& anim = animationName.second;
        if (activeAnimations.contains(name)) {
            continue;
        }

        keysToDelete.insert(name);
        auto list = !anim.iterator;
        if (list != nullptr) {
            for (Texture* frame : *list) {
                delete frame;
            }
            delete list;
        }
    }

    for (const string& key : keysToDelete) {
        animations.erase(key);
    }
}
