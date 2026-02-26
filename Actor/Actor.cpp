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

bool Actor::getSize(int& width, int& height, Error& err) const {
    width = 0;
    height = 0;
    if (currentAnimation.empty()) {
        err = Error::New("Actor has no current animation");
        return false;
    }
    auto actorAnimIt = animations.find(currentAnimation);
    if (actorAnimIt == animations.end()) {
        err = Error::New("Actor animation mapping not found");
        return false;
    }
    State& state = State::get();
    auto animIt = state.animations.find(actorAnimIt->second);
    if (animIt == state.animations.end()) {
        err = Error::New(string("Animation not loaded: ") + actorAnimIt->second);
        return false;
    }
    Animation& anim = animIt->second;
    auto list = !anim.iterator;
    if (list == nullptr || list->empty()) {
        err = Error::New(string("Animation has no frames: ") + actorAnimIt->second);
        return false;
    }
    Texture* frame = *anim.iterator;
    if (frame == nullptr) {
        err = Error::New(string("Animation frame is null: ") + actorAnimIt->second);
        return false;
    }
    const Vector* drawSize = frame->getDrawSize();
    if (drawSize != nullptr) {
        width = static_cast<int>((*drawSize)[0]);
        height = static_cast<int>((*drawSize)[1]);
        err = Error::Success();
        return true;
    }
    frame->querySize(width, height, err);
    if (err.status == failure) {
        err = Error::New(string("Failed querying frame size: ")+err.message);
        return false;
    }
    err = Error::Success();
    return true;
}
