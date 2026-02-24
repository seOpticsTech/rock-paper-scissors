//
// Created by user on 24/02/2026.
//

#include <string>
#include "MiniMe.h"
#include "State/State.h"

Actor* CreateMiniMe(const SDL_Rect& pos, Error& err) {
    auto& state = State::get();
    Animation anim;
    static int minimeCounter = 0;
    Actor* actor = state.addActor(string("minime")+string(minimeCounter), err);
    if (err.status == failure) {
        return nullptr;
    }
    state.loadAnimation("rock", {"./assets/rock.png"}, 0, err);
    if (err.status == failure && err.type != Error::duplicate) {
        return nullptr;
    }
    state.loadAnimation("paper", {"./assets/paper.png"}, 0, err);
    if (err.status == failure && err.type != Error::duplicate) {
        return nullptr;
    }
    state.loadAnimation("scissors", { "./assets/scissors.png"}, 0, err);
    if (err.status == failure && err.type != Error::duplicate) {
        return nullptr;
    }
    actor->animations["rock"] = "rock";
    actor->animations["paper"] = "paper";
    actor->animations["scissors"] = "scissors";
    actor->currentAnimation = "rock";
}