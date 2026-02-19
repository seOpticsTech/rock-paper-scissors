//
// Created by user on 19/02/2026.
//

#include "Player.h"
#include "State/State.h"
using namespace std;

Player::Player(State& state, Error& err) : actor(nullptr) {
    actor = state.addActor("player", err);
    if (err.status == failure) {
        return;
    }
    actor->textures["main"] = state.loadTexture("player", "./assets/player.png", err);
    if (err.status == failure) {
        return;
    }
    actor->currentTexture = "main";
    actor->position = Vector(380, 280);
    actor->acceleration = Vector(0, 0.1);
}

Player::~Player() = default;
