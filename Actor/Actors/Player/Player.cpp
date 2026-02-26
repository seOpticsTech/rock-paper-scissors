//
// Created by user on 19/02/2026.
//

#include "Player.h"
#include "State/State.h"
#include "Actor/Actors/MiniMe/MiniMe.h"
#include "Texture/Texture.h"
#include <iostream>
using namespace std;

void onRenderPlayer(Player& player) {
    if (player.hp <= 0) {
        State::get().removeActor(&player);
        return;
    }
    Uint32 nowMs = SDL_GetTicks();
    if (nowMs - player.lastModeChangeMs < 3000) {
        return;
    }
    Error err;
    int width = 0;
    int height = 0;
    State& state = State::get();
    if (!player.currentAnimation.empty()) {
        auto actorAnimIt = player.animations.find(player.currentAnimation);
        if (actorAnimIt != player.animations.end()) {
            auto animIt = state.animations.find(actorAnimIt->second);
            if (animIt != state.animations.end()) {
                Animation& anim = animIt->second;
                Texture* frame = *anim.iterator;
                if (frame != nullptr) {
                    const Vector* drawSize = frame->getDrawSize();
                    if (drawSize != nullptr) {
                        width = static_cast<int>((*drawSize)[0]);
                        height = static_cast<int>((*drawSize)[1]);
                    } else {
                        frame->querySize(width, height, err);
                        if (err.status == failure) {
                            return;
                        }
                    }
                }
            }
        }
    }

    if (state.view != nullptr) {
        int viewWidth = state.view->width;
        int viewHeight = state.view->height;
        double x = player.position[0];
        double y = player.position[1];
        if (x + width < 0) {
            player.position[0] = static_cast<double>(viewWidth);
        } else if (x > viewWidth) {
            player.position[0] = static_cast<double>(-width);
        }
        if (y + height < 0) {
            player.position[1] = static_cast<double>(viewHeight);
        } else if (y > viewHeight) {
            player.position[1] = static_cast<double>(-height);
        }
    }

    if (nowMs - player.lastSpawnMs < 800) {
        return;
    }
    if (width <= 0 || height <= 0) {
        width = 0;
        height = 0;
    }
    int offsetX = width / 4;
    int offsetY = height / 4;
    SDL_Rect pos{
        static_cast<int>(player.position[0]) + offsetX,
        static_cast<int>(player.position[1]) + offsetY,
        0,
        0
    };
    player.lastSpawnMs = nowMs;
    MiniMe* minime = new MiniMe(pos, player.mode, err);
    if (err.status == failure) {
        delete minime;
    }
}

Actor::eventAction Player::genOnKeyDown(const Vector& v) {
    return [this, v](Actor& actor, const SDL_Event& event) {
        static_cast<void>(actor);
        static_cast<void>(event);
        const Vector& current = this->velocity;
        const bool movingX = current[0] != 0.0;
        const bool movingY = current[1] != 0.0;
        const bool wantsX = v[0] != 0.0;
        const bool wantsY = v[1] != 0.0;
        if ((movingX && wantsX) || (movingY && wantsY)) {
            return;
        }
        this->velocity = v;
    };
}

Player::Player(const string& name, const Vector& startPosition, Error& err)
    : Actor(), mode(Rock), lastSpawnMs(0), lastModeChangeMs(0), hp(3) {
    State& state = State::get();
    if (state.actors.contains(name)) {
        err = Error::New(string("Player with name ") + name + " already exists", Error::duplicate);
        return;
    }

    state.loadAnimation("rock", {"./assets/rock.png"}, 0, err);
    if (err.status == failure) {
        return;
    }
    state.loadAnimation("paper", {"./assets/paper.png"}, 0, err);
    if (err.status == failure) {
        return;
    }
    state.loadAnimation("scissors", { "./assets/scissors.png"}, 0, err);
    if (err.status == failure) {
        return;
    }
    animations["rock"] = "rock";
    animations["paper"] = "paper";
    animations["scissors"] = "scissors";
    currentAnimation = "rock";

    position = startPosition;
    acceleration = Vector(0, 0);

    eventActions[Actor::keyDown][SDLK_LEFT] = genOnKeyDown(Vector(-5, 0));
    eventActions[Actor::keyDown][SDLK_RIGHT] = genOnKeyDown(Vector(5, 0));
    eventActions[Actor::keyDown][SDLK_UP] = genOnKeyDown(Vector(0, -5));
    eventActions[Actor::keyDown][SDLK_DOWN] = genOnKeyDown(Vector(0, 5));
    eventActions[Actor::keyDown][SDLK_z] = [this](Actor& actor, const SDL_Event& event) {
        static_cast<void>(event);
        Uint32 nowMs = SDL_GetTicks();
        if (nowMs - lastModeChangeMs < 3000) {
            return;
        }
        mode = Rock;
        currentAnimation = "rock";
        lastModeChangeMs = nowMs;
    };
    eventActions[Actor::keyDown][SDLK_x] = [this](Actor& actor, const SDL_Event& event) {
        static_cast<void>(event);
        Uint32 nowMs = SDL_GetTicks();
        if (nowMs - lastModeChangeMs < 3000) {
            return;
        }
        mode = Paper;
        currentAnimation = "paper";
        lastModeChangeMs = nowMs;
    };
    eventActions[Actor::keyDown][SDLK_c] = [this](Actor& actor, const SDL_Event& event) {
        static_cast<void>(event);
        Uint32 nowMs = SDL_GetTicks();
        if (nowMs - lastModeChangeMs < 3000) {
            return;
        }
        mode = Scissors;
        currentAnimation = "scissors";
        lastModeChangeMs = nowMs;
    };


    eventActions[Actor::quit][0] = [](Actor& actor, const SDL_Event& event) {
        cout << "Bye from player!" << endl;
    };

    onRender = [this](Actor& actor) {
        static_cast<void>(actor);
        onRenderPlayer(*this);
    };

    if (!state.registerActor(name, this, err)) {
        return;
    }
}

Player::~Player() = default;
