//
// Created by user on 26/02/2026.
//

#ifndef GAME_REMOTE_EVENT_HANDLING_H
#define GAME_REMOTE_EVENT_HANDLING_H

#include "Config/Config.h"
#include "Error.h"
#include <SDL2/SDL.h>
#include <cstdint>
#include <functional>

namespace RemoteEventHandling {

enum RemoteAction : uint32_t {
    RemoteActionLeft = 0,
    RemoteActionRight = 1,
    RemoteActionUp = 2,
    RemoteActionDown = 3,
    RemoteActionPaper = 4,
    RemoteActionRock = 5,
    RemoteActionScissors = 6,
    RemoteActionQuit = 7
};

bool init(const ConnectionDetails& details, Error& err);
void shutdown();
void configureLocalPollEvent(const function<int(SDL_Event*)>& pollEvent, ControlMode localControlMode);
int pollEvent(SDL_Event* event);

}

#endif //GAME_REMOTE_EVENT_HANDLING_H
