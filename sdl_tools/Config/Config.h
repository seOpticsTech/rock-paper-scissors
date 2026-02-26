//
// Created by user on 14/02/2026.
//

#ifndef GAME_CONFIG_H
#define GAME_CONFIG_H

#include <SDL2/SDL.h>
#include <functional>
#include <cstdint>
#include <string>

using namespace std;

enum ControlMode {
    KEYBOARD,
    GAMEPAD,
    REMOTE
};

struct ConnectionDetails {
    string remoteHost;
    uint16_t localPort;
    uint16_t remotePort;
};

struct Config {
    Uint32 sdlInitFlags;
    string windowTitle;
    int windowX;
    int windowY;
    int windowWidth;
    int windowHeight;
    Uint32 windowFlags;
    int rendererIndex;
    Uint32 rendererFlags;
    int imgInitFlags;
    int fps;
    function<int(SDL_Event *)> pollEvent;
    ControlMode controlMode;
    string localPlayer;
    bool toConnect;
    ConnectionDetails connectionDetails;
};

Config getDefaultConfig();

#endif //GAME_CONFIG_H
