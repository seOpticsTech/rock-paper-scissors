//
// Created by user on 14/02/2026.
//

#include "State.h"
#include "Actor/Actors/View/View.h"
#include "Actor/Actors/Player/Player.h"
#include "Actor/Actors/MiniMe/MiniMe.h"
#include "utils/Utils.h"
#include "RemoteEventHandling/RemoteEventHandling.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_gamecontroller.h>
#include <map>
#include <iostream>
#include <unordered_map>
#include <unordered_set>

#include "Actor/Actor.h"

using namespace std;

typedef function<Error(const SDL_Event&)> eventHandler;

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

function<Error(const SDL_Event&)> getEventHandler(Actor::EventGroup eventGroup, function<Uint32(const SDL_Event&)> getKey) {
    return [eventGroup, getKey](const SDL_Event& event) {
        State& state = State::get();
        if (eventGroup == Actor::remote && event.user.code == RemoteEventHandling::RemoteActionQuit) {
            state.running = false;
            return Error::Success();
        }
        for (pair<string, Actor*> nameActor : state.actors) {
            Actor* actor = nameActor.second;
            if (actor == nullptr) {
                continue;
            }
            auto groupIt = actor->eventActions.find(eventGroup);
            if (groupIt == actor->eventActions.end()) {
                continue;
            }
            Uint32 key = getKey(event);
            auto actionIt = groupIt->second.find(key);
            if (actionIt == groupIt->second.end() || actionIt->second == nullptr) {
                continue;
            }
            actionIt->second(*actor, event);
        }
        return Error::Success();
    };
}

State::State(const Config& config, Error& err)
    : animations(),
      env(nullptr),
      running(true),
      actors(),
      animationCleanupEveryXFrames(1000),
      view(nullptr),
      fps(config.fps),
      controlMode(config.controlMode),
      controllers(),
      controllerIds(),
      pollEvent(config.pollEvent),
      deferActorMutations(false),
      pendingAddActors(),
      pendingRemoveNames(),
      pendingRemoveActors() {
    env = new SDL_Environment(config, err);
    if (err.status == failure) {
        err = Error::New("Failed to set up SDL_Environment: " + err.message);
        return;
    }

    int requiredControllers = 0;
    if (controlMode == GAMEPAD) {
        requiredControllers = 1;
    }

    if (requiredControllers > 0) {
        const int numJoysticks = SDL_NumJoysticks();
        for (int i = 0; i < numJoysticks && static_cast<int>(controllers.size()) < requiredControllers; ++i) {
            if (!SDL_IsGameController(i)) {
                continue;
            }
            SDL_GameController* controller = SDL_GameControllerOpen(i);
            if (controller == nullptr) {
                err = Error::New(string("Failed to open game controller: ") + SDL_GetError());
                break;
            }
            SDL_Joystick* joystick = SDL_GameControllerGetJoystick(controller);
            SDL_JoystickID id = SDL_JoystickInstanceID(joystick);
            controllers.push_back(controller);
            controllerIds.push_back(id);
        }
        if (err.status == failure || static_cast<int>(controllers.size()) < requiredControllers) {
            if (err.status == success) {
                err = Error::New("Not enough game controllers found");
            }
            for (SDL_GameController* controller : controllers) {
                SDL_GameControllerClose(controller);
            }
            controllers.clear();
            controllerIds.clear();
            return;
        }
    }

    view = new View(1920, 1080);
    if (!registerActor("view", view, err)) {
        return;
    }
    view->eventActions[Actor::quit][0] = [this](Actor& actor, const SDL_Event& event) {
        running = false;
    };
}

State::~State() {
    for (pair<string, Actor*> actor : actors) {
        delete actor.second;
    }
    for (const auto& nameActor : pendingAddActors) {
        delete nameActor.second;
    }
    pendingAddActors.clear();
    pendingRemoveNames.clear();
    pendingRemoveActors.clear();
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
    for (SDL_GameController* controller : controllers) {
        SDL_GameControllerClose(controller);
    }
    controllers.clear();
    controllerIds.clear();
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
    typeToHandler[SDL_QUIT] = getEventHandler(Actor::quit, [](const SDL_Event& event) {
        static_cast<void>(event);
        return static_cast<Uint32>(0);
    });
    typeToHandler[SDL_KEYDOWN] = getEventHandler(Actor::keyDown, [](const SDL_Event& event) {
        return static_cast<Uint32>(event.key.keysym.sym);
    });
    typeToHandler[SDL_KEYUP] = getEventHandler(Actor::keyUp, [](const SDL_Event& event) {
        return static_cast<Uint32>(event.key.keysym.sym);
    });
    typeToHandler[SDL_CONTROLLERBUTTONDOWN] = getEventHandler(Actor::controllerButtonDown, [](const SDL_Event& event) {
        return static_cast<Uint32>(event.cbutton.button);
    });
    typeToHandler[SDL_CONTROLLERBUTTONUP] = getEventHandler(Actor::controllerButtonUp, [](const SDL_Event& event) {
        return static_cast<Uint32>(event.cbutton.button);
    });
    typeToHandler[SDL_USEREVENT] = getEventHandler(Actor::remote, [](const SDL_Event& event) {
        return static_cast<Uint32>(event.user.code);
    });

    int frameCounter = 0;
    unordered_map<string, Uint32> animationLastAdvanceMs;

    const double perfFrequency = static_cast<double>(SDL_GetPerformanceFrequency());

    deferActorMutations = true;
    while (running) {
        Uint64 frameStart = SDL_GetPerformanceCounter();
        while (pollEvent(&event)) {
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

        env->renderer->setDrawColor(16, 64, 32, 255);
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
                env->renderer->copy(frame, actor->position - view->position, err);
                if (err.status == failure) {
                    cerr << "Texture copy error: " << err.message << endl;
                }
                if (anim.msPerFrame <= 0) {
                    ++anim.iterator;
                } else {
                    Uint32 nowMs = SDL_GetTicks();
                    Uint32& lastAdvance = animationLastAdvanceMs[animationName];
                    if (nowMs - lastAdvance >= static_cast<Uint32>(anim.msPerFrame)) {
                        ++anim.iterator;
                        lastAdvance = nowMs;
                    }
                }
            }
        }

        env->renderer->present();
        collisionHandler();
        applyDeferredActors();
        if (frameCounter % animationCleanupEveryXFrames == 0) {
            cleanupAnimations();
        }
        frameCounter++;

        if (fps > 0) {
            const double targetSeconds = 1.0 / static_cast<double>(fps);
            double elapsedSeconds = (SDL_GetPerformanceCounter() - frameStart) / perfFrequency;
            if (elapsedSeconds < targetSeconds) {
                double remainingSeconds = targetSeconds - elapsedSeconds;
                Uint32 delayMs = static_cast<Uint32>(remainingSeconds * 1000.0);
                if (delayMs > 0) {
                    SDL_Delay(delayMs);
                }
                while (((SDL_GetPerformanceCounter() - frameStart) / perfFrequency) < targetSeconds) {
                }
            }
        }
    }
    deferActorMutations = false;
}

void State::collisionHandler() {
    vector<Player*> players;
    vector<MiniMe*> minimes;
    for (pair<string, Actor*> nameActor : actors) {
        Actor* actor = nameActor.second;
        if (actor == nullptr) {
            continue;
        }
        if (auto* player = dynamic_cast<Player*>(actor)) {
            players.push_back(player);
            continue;
        }
        if (auto* minime = dynamic_cast<MiniMe*>(actor)) {
            minimes.push_back(minime);
        }
    }

    handleMiniMesCollisions(players, minimes);
    handlePlayersCollisions(players);
}

void State::handleMiniMesCollisions(const vector<Player*>& players, const vector<MiniMe*>& minimes) {
    Error err;
    unordered_set<Actor*> toDelete;

    for (Player* player : players) {
        int playerWidth = 0;
        int playerHeight = 0;
        if (!player->getSize(playerWidth, playerHeight, err)) {
            if (err.status == failure) {
                cerr << "Player size error: " << err.message << endl;
            }
            continue;
        }
        for (MiniMe* minime : minimes) {
            if (toDelete.contains(minime)) {
                continue;
            }
            int minimeWidth = 0;
            int minimeHeight = 0;
            if (!minime->getSize(minimeWidth, minimeHeight, err)) {
                if (err.status == failure) {
                    cerr << "MiniMe size error: " << err.message << endl;
                }
                continue;
            }
            if (!rectanglesOverlap(player->position[0], player->position[1], playerWidth, playerHeight,
                                   minime->position[0], minime->position[1], minimeWidth, minimeHeight)) {
                continue;
            }
            int result = rpsResult(player->mode, minime->mode);
            if (result == 0) {
                continue;
            }
            if (result > 0) {
                toDelete.insert(minime);
            } else {
                player->hp -= 1;
                toDelete.insert(minime);
            }
        }
    }
    for (Actor* actor : toDelete) {
        removeActor(actor);
    }
}

void State::handlePlayersCollisions(const vector<Player*>& players) {
    if (players.size() < 2) {
        return;
    }
    Error err;
    for (size_t i = 0; i < players.size(); ++i) {
        Player* left = players[i];
        int leftWidth = 0;
        int leftHeight = 0;
        if (!left->getSize(leftWidth, leftHeight, err)) {
            if (err.status == failure) {
                cerr << "Player size error: " << err.message << endl;
            }
            continue;
        }
        for (size_t j = i + 1; j < players.size(); ++j) {
            Player* right = players[j];
            int rightWidth = 0;
            int rightHeight = 0;
            if (!right->getSize(rightWidth, rightHeight, err)) {
                if (err.status == failure) {
                    cerr << "Player size error: " << err.message << endl;
                }
                continue;
            }
            if (rectanglesOverlap(left->position[0], left->position[1], leftWidth, leftHeight,
                                  right->position[0], right->position[1], rightWidth, rightHeight)) {
                int result = rpsResult(left->mode, right->mode);
                if (result > 0) {
                    right->hp -= 3;
                } else if (result < 0) {
                    left->hp -= 3;
                }
            }
        }
    }
}

void State::removeActor(const string& name) {
    if (deferActorMutations) {
        pendingRemoveNames.insert(name);
        for (auto it = pendingAddActors.begin(); it != pendingAddActors.end(); ++it) {
            if (it->first == name) {
                delete it->second;
                pendingAddActors.erase(it);
                break;
            }
        }
        return;
    }
    auto it = actors.find(name);
    if (it == actors.end()) {
        return;
    }
    delete it->second;
    actors.erase(it);
}

void State::removeActor(Actor* actor) {
    if (actor == nullptr) {
        return;
    }
    if (deferActorMutations) {
        for (auto it = pendingAddActors.begin(); it != pendingAddActors.end(); ++it) {
            if (it->second == actor) {
                delete it->second;
                pendingAddActors.erase(it);
                return;
            }
        }
        pendingRemoveActors.insert(actor);
        return;
    }
    for (auto it = actors.begin(); it != actors.end(); ++it) {
        if (it->second == actor) {
            if (it->second == view) {
                view = nullptr;
            }
            delete it->second;
            actors.erase(it);
            return;
        }
    }
}

Actor* State::addActor(const string& name, Error& err) {
    auto a = new Actor();
    if (!registerActor(name, a, err)) {
        delete a;
        return nullptr;
    }
    return a;
}

bool State::registerActor(const string& name, Actor* actor, Error& err) {
    if (actor == nullptr) {
        err = Error::New("Actor is null");
        return false;
    }
    if (actors.contains(name)) {
        err = Error::New(string("Actor with name ") + name + " already exists", Error::duplicate);
        return false;
    }
    for (const auto& pending : pendingAddActors) {
        if (pending.first == name) {
            err = Error::New(string("Actor with name ") + name + " already exists", Error::duplicate);
            return false;
        }
    }
    if (deferActorMutations) {
        pendingAddActors.emplace_back(name, actor);
        return true;
    }
    actors[name] = actor;
    return true;
}

Animation State::loadAnimation(const string& name, std::initializer_list<string> paths, int msPerFrame, Error& err) {
    auto anim = loadAnimationNoDuplicate(name, paths, msPerFrame, err);
    if (err.status == failure) {
        if (err.type == Error::duplicate) {
            err = Error::Success();
        }
    }
    return anim;
}

Animation State::loadAnimationNoDuplicate(const string& name, std::initializer_list<string> paths, int msPerFrame, Error& err) {
    Animation anim;
    const auto& animIt = animations.find(name);
    if (animIt != animations.end()) {
        err = Error::New(string("Animation already exists: ") + name, Error::duplicate);
        anim = animIt->second;
        return anim;
    }
    anim = env->renderer->loadAnimation(paths, msPerFrame, err);
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

Animation State::loadAnimation(const string& name, std::initializer_list<pair<string, const SDL_Rect*>> frames, int msPerFrame, Error& err) {
    auto anim = loadAnimationNoDuplicate(name, frames, msPerFrame, err);
    if (err.status == failure) {
        if (err.type == Error::duplicate) {
            err = Error::Success();
        }
    }
    return anim;
}

Animation State::loadAnimationNoDuplicate(const string& name, std::initializer_list<pair<string, const SDL_Rect*>> frames, int msPerFrame, Error& err) {
    Animation anim;
    const auto& animIt = animations.find(name);
    if (animIt != animations.end()) {
        err = Error::New(string("Animation already exists: ") + name, Error::duplicate);
        anim = animIt->second;
        return anim;
    }
    anim = env->renderer->loadAnimation(frames, msPerFrame, err);
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

void State::applyDeferredActors() {
    if (pendingRemoveNames.empty() && pendingRemoveActors.empty() && pendingAddActors.empty()) {
        return;
    }

    for (const string& name : pendingRemoveNames) {
        auto it = actors.find(name);
        if (it == actors.end()) {
            continue;
        }
        if (it->second == view) {
            view = nullptr;
        }
        delete it->second;
        actors.erase(it);
    }

    for (Actor* actor : pendingRemoveActors) {
        for (auto it = actors.begin(); it != actors.end(); ++it) {
            if (it->second == actor) {
                if (it->second == view) {
                    view = nullptr;
                }
                delete it->second;
                actors.erase(it);
                break;
            }
        }
    }

    pendingRemoveNames.clear();
    pendingRemoveActors.clear();

    for (const auto& nameActor : pendingAddActors) {
        const string& name = nameActor.first;
        Actor* actor = nameActor.second;
        if (actors.contains(name)) {
            delete actor;
            continue;
        }
        actors[name] = actor;
    }
    pendingAddActors.clear();
}
