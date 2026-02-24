//
// Created by user on 14/02/2026.
//

#include "Actor.h"
#include "State/State.h"

Actor::Actor() :
    mass(0),
    position(),
    velocity(),
    acceleration(),
    currentAnimation(),
    animations(),
    eventActions(),
    onRender(nullptr),
    controlMode(KEYBOARD),
    controllerId(static_cast<SDL_JoystickID>(-1))
    {}

Actor::~Actor() = default;

bool Actor::getSize(State& state, int& width, int& height, Error& err) const {
    width = 0;
    height = 0;
    if (currentAnimation.empty()) {
        return false;
    }
    auto actorAnimIt = animations.find(currentAnimation);
    if (actorAnimIt == animations.end()) {
        return false;
    }
    auto animIt = state.animations.find(actorAnimIt->second);
    if (animIt == state.animations.end()) {
        return false;
    }
    Animation& anim = animIt->second;
    Texture* frame = *anim.iterator;
    if (frame == nullptr) {
        return false;
    }
    const Vector* drawSize = frame->getDrawSize();
    if (drawSize != nullptr) {
        width = static_cast<int>((*drawSize)[0]);
        height = static_cast<int>((*drawSize)[1]);
        return true;
    }
    frame->querySize(width, height, err);
    if (err.status == failure) {
        return false;
    }
    return true;
}
