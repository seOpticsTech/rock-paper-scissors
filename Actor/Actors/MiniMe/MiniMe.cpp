//
// Created by user on 24/02/2026.
//

#include <string>
#include "MiniMe.h"
#include "State/State.h"
#include "Texture/Texture.h"

using namespace std;

static void shrinkAnimationFrames(State& state, const string& animationName, Error& err) {
    auto animIt = state.animations.find(animationName);
    if (animIt == state.animations.end()) {
        err = Error::New(string("Missing animation: ") + animationName);
        return;
    }

    Animation& anim = animIt->second;
    CyclicList<Texture*>::cyclic_iterator start = anim.iterator;
    if (start == CyclicList<Texture*>::cyclic_iterator()) {
        err = Error::New(string("Missing animation frames: ") + animationName);
        return;
    }

    CyclicList<Texture*>::cyclic_iterator it = start;
    do {
        Texture* frame = *it;
        if (frame == nullptr) {
            ++it;
            continue;
        }
        int width = 0;
        int height = 0;
        frame->querySize(width, height, err);
        if (err.status == failure) {
            return;
        }
        int targetWidth = width / 2;
        int targetHeight = height / 2;
        if (targetWidth <= 0) {
            targetWidth = 1;
        }
        if (targetHeight <= 0) {
            targetHeight = 1;
        }
        frame->setDrawSize(targetWidth, targetHeight);
        ++it;
    } while (it != start);
}

MiniMe::MiniMe(const SDL_Rect& pos, Player::Mode playerMode, Error& err) : Actor(), mode(Rock) {
    auto& state = State::get();
    static int minimeCounter = 0;
    string name = string("minime_") + to_string(minimeCounter++);
    if (state.actors.contains(name)) {
        err = Error::New(string("Actor with name ") + name + " already exists", Error::duplicate);
        return;
    }
    position = Vector(pos.x, pos.y);
    acceleration = Vector(0, 0);

    const string rockAnim = "minime_rock";
    const string paperAnim = "minime_paper";
    const string scissorsAnim = "minime_scissors";

    state.loadAnimation(rockAnim, {"./assets/rock.png"}, 0, err);
    if (err.status == failure && err.type != Error::duplicate) {
        return;
    }
    if (err.status == failure) {
        err = Error::Success();
    } else {
        shrinkAnimationFrames(state, rockAnim, err);
        if (err.status == failure) {
            return;
        }
    }

    state.loadAnimation(paperAnim, {"./assets/paper.png"}, 0, err);
    if (err.status == failure && err.type != Error::duplicate) {
        return;
    }
    if (err.status == failure) {
        err = Error::Success();
    } else {
        shrinkAnimationFrames(state, paperAnim, err);
        if (err.status == failure) {
            return;
        }
    }

    state.loadAnimation(scissorsAnim, {"./assets/scissors.png"}, 0, err);
    if (err.status == failure && err.type != Error::duplicate) {
        return;
    }
    if (err.status == failure) {
        err = Error::Success();
    } else {
        shrinkAnimationFrames(state, scissorsAnim, err);
        if (err.status == failure) {
            return;
        }
    }

    animations["rock"] = rockAnim;
    animations["paper"] = paperAnim;
    animations["scissors"] = scissorsAnim;

    switch (playerMode) {
        case Player::Rock:
            mode = Rock;
            currentAnimation = "rock";
            break;
        case Player::Paper:
            mode = Paper;
            currentAnimation = "paper";
            break;
        case Player::Scissors:
            mode = Scissors;
            currentAnimation = "scissors";
            break;
    }

    state.actors[name] = this;
}

MiniMe::~MiniMe() = default;
