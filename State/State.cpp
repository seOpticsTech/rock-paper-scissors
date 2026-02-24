//
// Created by user on 14/02/2026.
//

#include "State.h"
#include "Actor/Actors/View/View.h"
#include "Actor/Actors/Player/Player.h"
#include "Actor/Actors/MiniMe/MiniMe.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_gamecontroller.h>
#include <map>
#include <iostream>
#include <unordered_map>
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
        if (actor->controlMode != KEYBOARD) {
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
        if (actor->controlMode != KEYBOARD) {
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

static bool mapControllerButtonToKey(const SDL_Event& event, SDL_Keycode& key) {
    if (event.type != SDL_CONTROLLERBUTTONDOWN && event.type != SDL_CONTROLLERBUTTONUP) {
        return false;
    }
    switch (event.cbutton.button) {
        case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
            key = SDLK_LEFT;
            return true;
        case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
            key = SDLK_RIGHT;
            return true;
        case SDL_CONTROLLER_BUTTON_DPAD_UP:
            key = SDLK_UP;
            return true;
        case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
            key = SDLK_DOWN;
            return true;
        case SDL_CONTROLLER_BUTTON_A:
            key = SDLK_z;
            return true;
        case SDL_CONTROLLER_BUTTON_B:
            key = SDLK_x;
            return true;
        case SDL_CONTROLLER_BUTTON_X:
            key = SDLK_c;
            return true;
        default:
            return false;
    }
}

Error handleControllerButtonEvent(const SDL_Event& event) {
    State& state = State::get();
    SDL_Keycode key;
    if (!mapControllerButtonToKey(event, key)) {
        return Error::Success();
    }
    Actor::EventGroup group = event.type == SDL_CONTROLLERBUTTONDOWN ? Actor::keyDown : Actor::keyUp;
    Uint32 keyCode = static_cast<Uint32>(key);
    for (pair<string, Actor*> nameActor : state.actors) {
        Actor* actor = nameActor.second;
        if (actor == nullptr) {
            continue;
        }
        if (actor->controlMode != GAMEPAD) {
            continue;
        }
        if (actor->controllerId != event.cbutton.which) {
            continue;
        }
        auto groupIt = actor->eventActions.find(group);
        if (groupIt == actor->eventActions.end()) {
            continue;
        }
        auto actionIt = groupIt->second.find(keyCode);
        if (actionIt == groupIt->second.end() || actionIt->second == nullptr) {
            continue;
        }
        actionIt->second(*actor, event);
    }
    return Error::Success();
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
      pollEvent(config.pollEvent) {
    env = new SDL_Environment(config, err);
    if (err.status == failure) {
        err = Error::New("Failed to set up SDL_Environment: " + err.message);
        return;
    }

    int requiredControllers = 0;
    if (controlMode == GAMEPAD) {
        requiredControllers = 2;
    } else {
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
    if (actors.contains("view")) {
        err = Error::New("Actor with name view already exists", Error::duplicate);
        return;
    }
    actors["view"] = view;
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
    typeToHandler[SDL_QUIT] = handleQuitEvent;
    typeToHandler[SDL_KEYDOWN] = handleKeyDownEvent;
    typeToHandler[SDL_KEYUP] = handleKeyUpEvent;
    typeToHandler[SDL_CONTROLLERBUTTONDOWN] = handleControllerButtonEvent;
    typeToHandler[SDL_CONTROLLERBUTTONUP] = handleControllerButtonEvent;

    int frameCounter = 0;
    unordered_map<string, Uint32> animationLastAdvanceMs;

    const double perfFrequency = static_cast<double>(SDL_GetPerformanceFrequency());

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
}

void State::removeActor(const string& name) {
    auto it = actors.find(name);
    if (it == actors.end()) {
        return;
    }
    if (it->second == view) {
        view = nullptr;
    }
    delete it->second;
    actors.erase(it);
}

void State::removeActor(Actor* actor) {
    if (actor == nullptr) {
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


bool rectanglesOverlap(double ax, double ay, int aw, int ah, double bx, double by, int bw, int bh) {
    return ax < bx + bw && ax + aw > bx && ay < by + bh && ay + ah > by;
}

int rpsResult(Player::Mode left, Player::Mode right) {
    if (left == right) {
        return 0;
    }
    if ((left == Player::Rock && right == Player::Scissors) ||
        (left == Player::Paper && right == Player::Rock) ||
        (left == Player::Scissors && right == Player::Paper)) {
        return 1;
        }
    return -1;
}

int rpsResult(Player::Mode left, MiniMe::Mode right) {
    if (left == Player::Rock && right == MiniMe::Rock) {
        return 0;
    }
    if (left == Player::Paper && right == MiniMe::Paper) {
        return 0;
    }
    if (left == Player::Scissors && right == MiniMe::Scissors) {
        return 0;
    }
    if ((left == Player::Rock && right == MiniMe::Scissors) ||
        (left == Player::Paper && right == MiniMe::Rock) ||
        (left == Player::Scissors && right == MiniMe::Paper)) {
        return 1;
        }
    return -1;
}

void State::collisionHandler() {
    Error err;
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

    unordered_set<Actor*> toDelete;

    for (Player* player : players) {
        int playerWidth = 0;
        int playerHeight = 0;
        if (!player->getSize(*this, playerWidth, playerHeight, err)) {
            continue;
        }
        for (MiniMe* minime : minimes) {
            if (toDelete.contains(minime)) {
                continue;
            }
            int minimeWidth = 0;
            int minimeHeight = 0;
            if (!minime->getSize(*this, minimeWidth, minimeHeight, err)) {
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

    if (players.size() >= 2) {
        Player* left = players[0];
        Player* right = players[1];
        if (!toDelete.contains(left) && !toDelete.contains(right)) {
            int leftWidth = 0;
            int leftHeight = 0;
            int rightWidth = 0;
            int rightHeight = 0;
            if (left->getSize(*this, leftWidth, leftHeight, err) &&
                right->getSize(*this, rightWidth, rightHeight, err)) {
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

    for (Actor* actor : toDelete) {
        removeActor(actor);
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

Animation State::loadAnimation(const string& name, std::initializer_list<string> paths, int msPerFrame, Error& err) {
    if (animations.find(name) != animations.end()) {
        err = Error::New(string("Animation already exists: ") + name, Error::duplicate);
        return {};
    }
    Animation anim = env->renderer->loadAnimation(paths, msPerFrame, err);
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
    if (animations.find(name) != animations.end()) {
        err = Error::New(string("Animation already exists: ") + name, Error::duplicate);
        return {};
    }
    Animation anim = env->renderer->loadAnimation(frames, msPerFrame, err);
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
