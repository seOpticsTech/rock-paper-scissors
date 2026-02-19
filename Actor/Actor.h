//
// Created by user on 14/02/2026.
//

#ifndef GAME_ACTOR_H
#define GAME_ACTOR_H
#include "Texture/Texture.h"
#include "Vector/Vector.h"
#include <SDL2/SDL.h>
#include <map>
#include <memory>
#include <string>

using namespace std;


class Actor {
public:
    using keyDownAction = void (*)(Actor&, const SDL_Event&);

    Actor();
    ~Actor();

    double mass;
    Vector position;
    Vector velocity;
    Vector acceleration;

    string currentTexture;
    map<string, Texture*> textures;
    map<SDL_Keycode, keyDownAction> keyDownActions;
};


#endif //GAME_ACTOR_H
