cmake_minimum_required(VERSION 4.1)
project(game)

set(CMAKE_CXX_STANDARD 20)

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)

add_executable(game main.cpp)

target_link_libraries(game
        SDL2::SDL2
        SDL2_image::SDL2_image
)

target_compile_options(game PRIVATE
        -Wall
        -Wextra
        -Wpedantic
)