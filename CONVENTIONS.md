# Project Conventions

## Data Visibility
- Variables are public by default. Make them private only when there is a clear reason.
- No getters/setters unless a member truly needs encapsulation.

## Naming
- Classes use PascalCase: `Window`, `Renderer`, `SDL_Environment`, `Texture`, `Error`.
- Member functions use lower camelCase: `loadTexture`, `querySize`, `setDrawColor`.
- Member variables are plain names without prefixes: `window`, `renderer`, `texture`.
- Error status uses `Status` enum values: `success`, `failure`.

## Files and Folders
- File names match class names: `Renderer.h`/`Renderer.cpp`, `SDL_Environment.h`/`SDL_Environment.cpp`.
- Class folders are PascalCase under `sdl_tools/` (e.g., `sdl_tools/Renderer/`).
- Shared non-SDL utilities live at project root (e.g., `Error/`).

## C++ Style
- 4-space indentation.
- Opening braces on the same line as declarations/definitions.
- `using namespace std;` is used in headers and source files.
- Include guards use `GAME_<NAME>_H` style.
- Prefer `const auto` for local variables when appropriate.
- Raw pointers are used with explicit ownership and cleanup in destructors.
- `Status` is an unscoped enum with `success`/`failure` in the global namespace.

## Error Handling
- Use `Error` as the standard error channel (`status` + `message`).
- Functions only set `err` on failure; they do not clear it on success.
- SDL failures should map to `Error::New` with the SDL error string.
