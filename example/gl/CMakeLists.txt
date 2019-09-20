cmake_minimum_required(VERSION 2.8)
project(cpp-sdl2-opengl-example)

set(CMAKE_CXX_STANDARD 17)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/../cmake-modules)
message(${CMAKE_MODULE_PATH})
find_package(SDL2 REQUIRED)
find_package(OpenGL REQUIRED)

#option(use_vulkan "set to true if you want to build vulkan wrappers" false)
#if(use_vulkan)
#	find_package(Vulkan REQUIRED)
#endif()


include_directories(
../../sources/ 
./glad/include/)

file(GLOB cppsdl2headers ../sources/cpp-sdl2/*.hpp)
add_executable(example main.cpp glad/src/glad.c ${cppsdl2headers})

target_link_libraries(example SDL2::SDL2 SDL2::SDL2main OpenGL::GL)
#if(use_vulkan)
#	target_link_libraries(example Vulkan::Vulkan)
#	target_compile_definitions(example CPP_SDL2_VK_WINDOW)
#endif()

