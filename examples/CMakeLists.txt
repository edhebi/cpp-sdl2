cmake_minimum_required(VERSION 2.8)
project(cpp-sdl2-examples)

option(CPP-SDL2_ENABLE_VULKAN "Build the Vulkan based examples" on)

add_subdirectory(gl)
if(CPP-SDL2_ENABLE_VULKAN)
  add_subdirectory(vk)
endif()
add_subdirectory(general)
add_subdirectory(dll)

file(COPY ${SDL2_RUNTIME_LIBRARY} DESTINATION ${CMAKE_BINARY_DIR})
