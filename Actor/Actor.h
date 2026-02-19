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
#include <functional>

using namespace std;


class Actor {
public:
    using eventAction = function<void(Actor&, const SDL_Event&)>;

    enum EventGroup {
        keyDown,
        keyUp,
        quit,
        windowEvent,
        mouseMotion,
        mouseButtonDown,
        mouseButtonUp,
        mouseWheel,
        textInput,
        textEditing,
        joystick,
        controller,
        touch,
        gesture,
        drop,
        userEvent,
        audioDevice
    };

    Actor();
    ~Actor();

    double mass;
    Vector position;
    Vector velocity;
    Vector acceleration;

    string currentTexture;
    map<string, Texture*> textures;
    map<EventGroup, map<Uint32, eventAction>> eventActions;
};


#endif //GAME_ACTOR_H
