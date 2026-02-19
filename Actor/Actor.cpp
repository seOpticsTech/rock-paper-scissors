//
// Created by user on 14/02/2026.
//

#include "Actor.h"

Actor::Actor() :
    mass(0),
    position(),
    velocity(),
    acceleration(),
    currentTexture(),
    textures(),
    eventActions()
    {}

Actor::~Actor() = default;
