#include <SDL2/SDL.h>
#include <iostream>

using namespace std;

int main(int argc, char *argv[])
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        cerr << "SDL_Init error: " << SDL_GetError() << endl;

        return 1;
    }

    SDL_Window *window = SDL_CreateWindow(
        "SDL2 Window",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        800,
        600,
        SDL_WINDOW_SHOWN
    );

    if (!window) {
        cerr << "SDL_CreateWindow error: " << SDL_GetError() << endl;
        SDL_Quit();
        return 1;
    }

    int running = 1;
    SDL_Event event;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;
            }
        }

        SDL_Delay(16);  // ~60 FPS idle loop
    }

    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
