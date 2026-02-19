//
// Created by user on 14/02/2026.
//

#ifndef GAME_ACTOR_H
#define GAME_ACTOR_H
#include "Texture/Texture.h"
#include "Vector/Vector.h"
#include <map>
#include <memory>
#include <string>


class Actor {
public:
    Actor();
    ~Actor();

    double mass;
    Vector position;
    Vector velocity;
    Vector acceleration;

    string currentTexture;
    map<string, Texture*> textures;
};


#endif //GAME_ACTOR_H