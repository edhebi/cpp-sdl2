cmake_minimum_required(VERSION 2.8)
project(cpp-sdl2-vulkan-example)

set(CMAKE_CXX_STANDARD 17)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/../cmake-modules)
message(${CMAKE_MODULE_PATH})
find_package(SDL2 REQUIRED)
find_package(Vulkan REQUIRED)

include_directories(../../sources/)

file(GLOB cppsdl2headers ../sources/cpp-sdl2/*.hpp)
file(GLOB shader_code ./*.glsl)
add_executable(example main.cpp ${shader_code} ${cppsdl2headers})

target_link_libraries(example SDL2::SDL2 SDL2::SDL2main)
target_link_libraries(example Vulkan::Vulkan)
