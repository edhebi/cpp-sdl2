# cpp-sdl2 examples cmake modules

In order to build the examples, we obviously need to locate some libaries.

modern CMake ships with everything needed to locate OpenGL libraries and Vulkan libraries, but the scripts for SDL2 aren't consistant accross operating systems, thus we ship our own `FindSDL2.cmake` file for convinience
