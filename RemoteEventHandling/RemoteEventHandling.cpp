//
// Created by user on 26/02/2026.
//

#include "RemoteEventHandling.h"
#include <arpa/inet.h>
#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

using namespace std;

namespace RemoteEventHandling {

static int socketFd = -1;
static sockaddr_in remoteAddr{};
static bool hasRemoteAddr = false;
static function<int(SDL_Event*)> localPollEvent;
static ControlMode localMode = KEYBOARD;

static bool setNonBlocking(int fd, Error& err) {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1) {
        err = Error::New(string("fcntl get flags failed: ") + strerror(errno));
        return false;
    }
    if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1) {
        err = Error::New(string("fcntl set nonblocking failed: ") + strerror(errno));
        return false;
    }
    return true;
}

bool init(const ConnectionDetails& details, Error& err) {
    if (socketFd != -1) {
        return true;
    }
    socketFd = socket(AF_INET, SOCK_DGRAM, 0);
    if (socketFd == -1) {
        err = Error::New(string("Failed to create UDP socket: ") + strerror(errno));
        return false;
    }
    if (!setNonBlocking(socketFd, err)) {
        shutdown();
        return false;
    }

    sockaddr_in localAddr{};
    localAddr.sin_family = AF_INET;
    localAddr.sin_addr.s_addr = INADDR_ANY;
    localAddr.sin_port = htons(details.localPort);
    if (bind(socketFd, reinterpret_cast<sockaddr*>(&localAddr), sizeof(localAddr)) == -1) {
        err = Error::New(string("Failed to bind UDP socket: ") + strerror(errno));
        shutdown();
        return false;
    }

    remoteAddr = {};
    remoteAddr.sin_family = AF_INET;
    remoteAddr.sin_port = htons(details.remotePort);
    if (inet_pton(AF_INET, details.remoteHost.c_str(), &remoteAddr.sin_addr) != 1) {
        err = Error::New("Invalid remote host address");
        shutdown();
        return false;
    }
    hasRemoteAddr = true;
    return true;
}

void shutdown() {
    if (socketFd != -1) {
        close(socketFd);
        socketFd = -1;
    }
    hasRemoteAddr = false;
}

void configureLocalPollEvent(const function<int(SDL_Event*)>& pollEvent, ControlMode localControlMode) {
    localPollEvent = pollEvent;
    localMode = localControlMode;
}

static bool sendAction(RemoteAction action, Error& err) {
    if (socketFd == -1 || !hasRemoteAddr) {
        return false;
    }
    uint32_t payload = htonl(static_cast<uint32_t>(action));
    ssize_t sent = sendto(socketFd, &payload, sizeof(payload), 0,
                          reinterpret_cast<sockaddr*>(&remoteAddr), sizeof(remoteAddr));
    if (sent != static_cast<ssize_t>(sizeof(payload))) {
        err = Error::New(string("Failed to send UDP action: ") + strerror(errno));
        return false;
    }
    return true;
}

static bool mapLocalEventToAction(const SDL_Event& event, RemoteAction& action) {
    if (event.type == SDL_QUIT) {
        action = RemoteActionQuit;
        return true;
    }
    if (localMode == KEYBOARD && event.type == SDL_KEYDOWN) {
        switch (event.key.keysym.sym) {
            case SDLK_LEFT:
                action = RemoteActionLeft;
                return true;
            case SDLK_RIGHT:
                action = RemoteActionRight;
                return true;
            case SDLK_UP:
                action = RemoteActionUp;
                return true;
            case SDLK_DOWN:
                action = RemoteActionDown;
                return true;
            case SDLK_x:
                action = RemoteActionPaper;
                return true;
            case SDLK_z:
                action = RemoteActionRock;
                return true;
            case SDLK_c:
                action = RemoteActionScissors;
                return true;
            default:
                return false;
        }
    }
    if (localMode == GAMEPAD && event.type == SDL_CONTROLLERBUTTONDOWN) {
        switch (event.cbutton.button) {
            case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
                action = RemoteActionLeft;
                return true;
            case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
                action = RemoteActionRight;
                return true;
            case SDL_CONTROLLER_BUTTON_DPAD_UP:
                action = RemoteActionUp;
                return true;
            case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
                action = RemoteActionDown;
                return true;
            case SDL_CONTROLLER_BUTTON_B:
                action = RemoteActionPaper;
                return true;
            case SDL_CONTROLLER_BUTTON_A:
                action = RemoteActionRock;
                return true;
            case SDL_CONTROLLER_BUTTON_X:
                action = RemoteActionScissors;
                return true;
            default:
                return false;
        }
    }
    return false;
}

static bool pollRemoteEvent(SDL_Event* event) {
    if (socketFd == -1 || event == nullptr) {
        return false;
    }
    uint32_t payload = 0;
    sockaddr_in senderAddr{};
    socklen_t senderLen = sizeof(senderAddr);
    ssize_t received = recvfrom(socketFd, &payload, sizeof(payload), 0,
                                reinterpret_cast<sockaddr*>(&senderAddr), &senderLen);
    if (received == -1) {
        if (errno == EWOULDBLOCK || errno == EAGAIN) {
            return false;
        }
        return false;
    }
    if (received != static_cast<ssize_t>(sizeof(payload))) {
        return false;
    }
    uint32_t action = ntohl(payload);
    event->type = SDL_USEREVENT;
    event->user.code = static_cast<Sint32>(action);
    event->user.data1 = nullptr;
    event->user.data2 = nullptr;
    return true;
}

int pollEvent(SDL_Event* event) {
    if (event == nullptr) {
        return 0;
    }
    if (pollRemoteEvent(event)) {
        return 1;
    }
    if (!localPollEvent) {
        return 0;
    }
    int result = localPollEvent(event);
    if (result <= 0) {
        return result;
    }
    if (localMode != REMOTE) {
        RemoteAction action;
        if (mapLocalEventToAction(*event, action)) {
            Error err;
            sendAction(action, err);
        }
    }
    return result;
}

}
