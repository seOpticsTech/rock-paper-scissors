#include <iostream>
#include <cctype>
#include <SDL2/SDL.h>
#include "Error.h"
#include "Actor/Actors/Player/Player.h"
#include "Config/Config.h"
#include "RemoteEventHandling/RemoteEventHandling.h"
#include "SDL_Environment/SDL_Environment.h"
#include "State/State.h"
#include "Vector/Vector.h"

using namespace std;

int customPollEvent(SDL_Event *event) {
    static bool eventSet = false;
    static Uint32 lastEventMs = 0;
    static bool leftNext = true;
    SDL_Event quitEvent;
    if (SDL_PollEvent(&quitEvent) && quitEvent.type == SDL_QUIT) {
        *event = quitEvent;
        return 1;
    }
    if (eventSet) {
        eventSet = false;
        return 0;
    }
    Uint32 nowMs = SDL_GetTicks();
    if (nowMs - lastEventMs >= 1000) {
        event->type = SDL_KEYDOWN;
        event->key.state = SDL_PRESSED;
        event->key.repeat = 0;
        event->key.keysym.sym = leftNext ? SDLK_LEFT : SDLK_RIGHT;
        event->key.keysym.scancode = SDL_GetScancodeFromKey(event->key.keysym.sym);
        event->key.keysym.mod = 0;
        leftNext = !leftNext;
        lastEventMs = nowMs;
        eventSet = true;
        return 1;
    }
    return 0;
}

static bool parseBool(const string& value, bool& result) {
    string lowered;
    lowered.reserve(value.size());
    for (char c : value) {
        lowered.push_back(static_cast<char>(tolower(c)));
    }
    if (lowered == "true" || lowered == "1" || lowered == "yes") {
        result = true;
        return true;
    }
    if (lowered == "false" || lowered == "0" || lowered == "no") {
        result = false;
        return true;
    }
    return false;
}

static bool parseUint16(const string& value, uint16_t& result) {
    try {
        int parsed = stoi(value);
        if (parsed < 0 || parsed > 65535) {
            return false;
        }
        result = static_cast<uint16_t>(parsed);
        return true;
    } catch (const exception&) {
        return false;
    }
}

static bool parseControlMode(const string& value, ControlMode& result) {
    if (value == "KEYBOARD") {
        result = KEYBOARD;
        return true;
    }
    if (value == "GAMEPAD") {
        result = GAMEPAD;
        return true;
    }
    if (value == "REMOTE") {
        result = REMOTE;
        return true;
    }
    return false;
}

static bool readArgValue(int& index, int argc, char** argv, const string& key, string& out) {
    string arg = argv[index];
    string prefix = key + "=";
    if (arg.rfind(prefix, 0) == 0) {
        out = arg.substr(prefix.size());
        return true;
    }
    if (arg == key && index + 1 < argc) {
        out = argv[++index];
        return true;
    }
    return false;
}

int main(int argc, char** argv)
{
    Error err;
    auto config = getDefaultConfig();
    // config.pollEvent = customPollEvent;

    for (int i = 1; i < argc; ++i) {
        string value;
        if (readArgValue(i, argc, argv, "--controlMode", value)) {
            if (!parseControlMode(value, config.controlMode)) {
                cerr << "Invalid controlMode: " << value << endl;
                return 1;
            }
            continue;
        }
        if (readArgValue(i, argc, argv, "--localPlayer", value)) {
            config.localPlayer = value;
            continue;
        }
        if (readArgValue(i, argc, argv, "--toConnect", value)) {
            if (!parseBool(value, config.toConnect)) {
                cerr << "Invalid toConnect value: " << value << endl;
                return 1;
            }
            continue;
        }
        if (readArgValue(i, argc, argv, "--remoteHost", value)) {
            config.connectionDetails.remoteHost = value;
            continue;
        }
        if (readArgValue(i, argc, argv, "--localPort", value)) {
            if (!parseUint16(value, config.connectionDetails.localPort)) {
                cerr << "Invalid localPort: " << value << endl;
                return 1;
            }
            continue;
        }
        if (readArgValue(i, argc, argv, "--remotePort", value)) {
            if (!parseUint16(value, config.connectionDetails.remotePort)) {
                cerr << "Invalid remotePort: " << value << endl;
                return 1;
            }
            continue;
        }
    }

    if (config.toConnect) {
        if (!RemoteEventHandling::init(config.connectionDetails, err)) {
            cerr << "Failed to set up remote events: " << err.message << endl;
            return 1;
        }
        RemoteEventHandling::configureLocalPollEvent(config.pollEvent, config.controlMode);
        config.pollEvent = RemoteEventHandling::pollEvent;
    }

    State::make(config, err);
    if (err.status == failure) {
        cerr << "SDL_Environment create error: " << err.message << endl;
        return 1;
    }
    State& state = State::get();

    Player* p1 = new Player("player_1", Vector(480, 540), err);
    if (err.status == failure) {
        delete p1;
        cerr << "Failed to create player 1: " << err.message << endl;
        return 1;
    }

    Player* p2 = new Player("player_2", Vector(1440, 540), err);
    if (err.status == failure) {
        delete p2; 
        cerr << "Failed to create player 2: " << err.message << endl;
        return 1;
    }

    Player* localPlayer = config.localPlayer == "player_2" ? p2 : p1;
    Player* remotePlayer = localPlayer == p1 ? p2 : p1;
    localPlayer->controlMode = config.controlMode;
    if (config.controlMode == GAMEPAD) {
        localPlayer->controllerId = state.controllerIds[0];
    }
    remotePlayer->controlMode = REMOTE;

    state.startEventLoop();
    State::destroy();
    if (config.toConnect) {
        RemoteEventHandling::shutdown();
    }
    return 0;
}
