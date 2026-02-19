cmake_minimum_required(VERSION 4.1)
project(game)

set(CMAKE_CXX_STANDARD 20)

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)

add_executable(game main.cpp
        Matrix/Matrix.cpp
        Matrix/Matrix.h
        Vector/Vector.cpp
        Vector/Vector.h
        sdl_tools/Config/Config.cpp
        sdl_tools/Config/Config.h
        sdl_tools/Window/Window.cpp
        sdl_tools/Window/Window.h
        sdl_tools/Renderer/Renderer.cpp
        sdl_tools/Renderer/Renderer.h
        sdl_tools/SDL_Environment/SDL_Environment.cpp
        sdl_tools/SDL_Environment/SDL_Environment.h
        Error/Error.cpp
        Error/Error.h
        sdl_tools/Texture/Texture.cpp
        sdl_tools/Texture/Texture.h
        Actor/Actor.cpp
        Actor/Actor.h
        State/State.cpp
        State/State.h
        Actor/Actors/Player/Player.cpp
        Actor/Actors/Player/Player.h
        Actor/Actors/View/View.cpp
        Actor/Actors/View/View.h
)

target_link_libraries(game
        SDL2::SDL2
        SDL2_image::SDL2_image
)

target_include_directories(game PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/sdl_tools
        ${CMAKE_CURRENT_SOURCE_DIR}/Error
)

target_compile_options(game PRIVATE
        -Wall
        -Wextra
        -Wpedantic
)
