cmake_minimum_required(VERSION 2.8)

set(CMAKE_CXX_STANDARD 17)

project(cpp-sdl2-examples)

option(CPP-SDL2_ENABLE_VULKAN "Build the Vulkan based examples" off)
option(CPP-SDL2_ENABLE_OPENGL "Build the OpenGL based examples" off)

if(CPP-SDL2_ENABLE_OPENGL)
  add_subdirectory(gl)
endif()
if(CPP-SDL2_ENABLE_VULKAN)
  add_subdirectory(vk)
endif()
add_subdirectory(general)
add_subdirectory(dll)

file(COPY ${SDL2_RUNTIME_LIBRARY} DESTINATION ${CMAKE_BINARY_DIR})
