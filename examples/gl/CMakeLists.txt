cmake_minimum_required(VERSION 2.8)
project(cpp-sdl2-opengl-example)

set(CMAKE_CXX_STANDARD 17)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/../cmake-modules)

find_package(SDL2 REQUIRED)
find_package(OpenGL REQUIRED)

include_directories(
../../sources/ 
./glad/include/)

file(GLOB cppsdl2headers ../sources/cpp-sdl2/*.hpp)
add_executable(gl-example main.cpp glad/src/glad.c ${cppsdl2headers})

