# Project Overview

This project is a small SDL2-based game scaffold centered around an event loop, an `Actor` system, and simple sprite animations (rock/paper/scissors). It wraps SDL setup into a few helper classes, provides a basic math layer (`Matrix`/`Vector`), and uses a cyclic list to loop animation frames. The executable is built as `game` via CMake.

## Entry Point

- `main.cpp` creates the default `Config`, initializes the singleton `State`, spawns a `Player`, and starts the event loop.
- There is an optional `customPollEvent` helper (currently commented out) that can synthesize left/right key presses every second, useful for automated movement testing.

## Core Runtime

### State

- `State` is a singleton owning the `SDL_Environment`, a global actor registry, and a map of loaded animations.
- `State::startEventLoop()`:
  - Polls events using `Config::pollEvent` (defaults to `SDL_PollEvent`).
  - Dispatches `SDL_QUIT`, `SDL_KEYDOWN`, and `SDL_KEYUP` to per-actor handlers.
  - Updates each actor's physics (`position += velocity`, `velocity += acceleration`).
  - Renders the current animation frame, offset by the `View` actor.
  - Advances animation frames either each render (msPerFrame == 0) or by elapsed ticks.
  - Performs animation cleanup every `animationCleanupEveryXFrames` frames.
- `State::loadAnimation()` loads and caches a named animation; if the name already exists, it returns a `duplicate` error.
- `State::cleanupAnimations()` removes animations not referenced by any actor's animation map and deletes their frames.

### Actor

- `Actor` is the base unit with physics vectors, a current animation key, and a per-event action map.
- Event actions are stored as `map<EventGroup, map<Uint32, eventAction>>`.
- `onRender` can be assigned for custom per-frame rendering behavior.

### View

- `View` is a lightweight camera concept. A `View` actor is created in `State` and used to offset render positions.

## Game Actors

### Player

- `Player` registers a `player` actor, loads the rock/paper/scissors animations, and sets the default animation to `rock`.
- Key events map arrow keys to velocity changes (±5 per tick); key-up resets velocity to zero.
- A quit event prints a short goodbye message.

### MiniMe

- `MiniMe` aims to create additional actors, but the implementation is incomplete:
  - `MiniMe.h` declares `Actor& CreateMiniMe(const SDL_Rect& pos);`
  - `MiniMe.cpp` defines `Actor* CreateMiniMe(const SDL_Rect& pos, Error& err)` with a different signature and returns `nullptr` on failure without returning the created actor on success.
- This code is not referenced elsewhere in the current build.

## SDL Wrappers (`sdl_tools/`)

- `Config` defines all SDL/SDL_image initialization and window/renderer parameters. `getDefaultConfig()` sets typical defaults (800x600, accelerated renderer, PNG support, 60 FPS).
- `SDL_Environment` owns SDL initialization and cleanup, and wires together `Window` and `Renderer`.
- `Window` is a thin wrapper around `SDL_CreateWindow` with move semantics.
- `Renderer` wraps `SDL_Renderer` and provides:
  - texture loading via SDL_image
  - animation loading into a `CyclicList<Texture*>`
  - basic render operations (`setDrawColor`, `clear`, `copy`, `present`)
- `Texture` wraps `SDL_Texture` and an optional clipping `SDL_Rect`.

## Animation and Data Structures

- `Animation` stores a cyclic iterator over a `CyclicList<Texture*>` and a frame delay (`msPerFrame`).
- `CyclicList` is a custom doubly-linked cyclic list with three iterator types:
  - `iterator`/`const_iterator` for linear traversal with `end()`
  - `cyclic_iterator` for infinite looping (used by animations)

## Math Utilities

- `Matrix` is a small dynamic matrix implementation supporting add/sub/multiply/transpose and identity.
- `Vector` is a 2x1 wrapper around `Matrix` with vector arithmetic and matrix interop.
- Both propagate errors through a shared `Error` type when dimensions mismatch.

## Error Handling

- `Error` is the shared error channel (status + message + type).
- `Status` is a global enum: `success` or `failure`.
- `Error::New()` creates failures; `Error::Success()` creates a success value.

## Assets

- `assets/` contains `rock.png`, `paper.png`, `scissors.png` and is referenced directly by `Player` and `MiniMe`.
- The directory is currently listed in `.gitignore`, so the build expects these files to exist locally even though they are ignored by git.

## Build

- `CMakeLists.txt` builds a single target `game` with C++20 and links `SDL2` + `SDL2_image`.
- Source files include the SDL wrappers, actor system, and math utilities.

## Conventions

- See `CONVENTIONS.md` for naming, error handling, and style rules (public-by-default, no getters/setters unless needed, etc.).
