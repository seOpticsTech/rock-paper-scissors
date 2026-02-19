//
// Created by user on 19/02/2026.
//

#include "Player.h"
#include "State/State.h"
#include <iostream>
#include <bits/codecvt.h>
using namespace std;

void onKeyUp(Actor& actor, const SDL_Event& event) {
    static_cast<void>(event);
    actor.velocity = Vector(0, 0);
}

Actor::eventAction Player::genOnKeyDown(const Vector& v) {
    return [this, v](Actor& actor, const SDL_Event& event) {
        static_cast<void>(actor);
        static_cast<void>(event);
        this->actor->velocity = v;
    };
}

Player::Player(Error& err) : actor(nullptr) {
    State& state = State::get();
    actor = state.addActor("player", err);
    if (err.status == failure) {
        return;
    }

    state.loadTexture("player", "./assets/player.png", err);
    if (err.status == failure && err.type != Error::duplicate) {
        return;
    }
    actor->textures["main"] = "player";
    actor->currentTexture = "main";

    actor->position = Vector(380, 280);
    actor->acceleration = Vector(0, 0);

    actor->eventActions[Actor::keyDown][SDLK_LEFT] = genOnKeyDown(Vector(-5, 0));
    actor->eventActions[Actor::keyDown][SDLK_RIGHT] = genOnKeyDown(Vector(5, 0));
    actor->eventActions[Actor::keyDown][SDLK_UP] = genOnKeyDown(Vector(0, -5));
    actor->eventActions[Actor::keyDown][SDLK_DOWN] = genOnKeyDown(Vector(0, 5));

    actor->eventActions[Actor::keyUp][SDLK_LEFT] = onKeyUp;
    actor->eventActions[Actor::keyUp][SDLK_RIGHT] = onKeyUp;
    actor->eventActions[Actor::keyUp][SDLK_UP] = onKeyUp;
    actor->eventActions[Actor::keyUp][SDLK_DOWN] = onKeyUp;

    actor->eventActions[Actor::quit][0] = [](Actor& actor, const SDL_Event& event) {
        cout << "Bye from player!" << endl;
    };
}

Player::~Player() = default;
