cmake_minimum_required(VERSION 2.8)
project(cpp-sdl2-general-dll-example)

set(CMAKE_CXX_STANDARD 17)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/../cmake-modules)

find_package(SDL2 REQUIRED)

include_directories(../../sources/)

file(GLOB cppsdl2headers ../../sources/cpp-sdl2/*.hpp)

add_executable(dll-example main.cpp ${cppsdl2headers})

target_link_libraries(dll-example PRIVATE SDL2::SDL2 SDL2::SDL2main)

  if (CMAKE_VERSION VERSION_GREATER 3.8.0 AND MSVC)
	set_target_properties(dll-example PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}")
  endif()


add_library(my_dll SHARED dll.c)

