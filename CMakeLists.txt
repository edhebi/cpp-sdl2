cmake_minimum_required(VERSION 3.12)

project(cpp_sdl2
	VERSION 0.1.0
	DESCRIPTION "Basic C++17 bindings to SDL2"
	HOMEPAGE_URL "https://github.com/Edhebi/cpp-sdl2"
	LANGUAGES C CXX
	)

set(CPP_SDL2_SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/sources/cpp-sdl2/color.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/sources/cpp-sdl2/event.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/sources/cpp-sdl2/exception.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/sources/cpp-sdl2/game_controller.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/sources/cpp-sdl2/haptic.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/sources/cpp-sdl2/joystick.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/sources/cpp-sdl2/mouse.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/sources/cpp-sdl2/pixel.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/sources/cpp-sdl2/rect.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/sources/cpp-sdl2/renderer.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/sources/cpp-sdl2/sdl.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/sources/cpp-sdl2/shared_object.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/sources/cpp-sdl2/simd.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/sources/cpp-sdl2/surface.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/sources/cpp-sdl2/texture.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/sources/cpp-sdl2/timer.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/sources/cpp-sdl2/utils.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/sources/cpp-sdl2/vec2.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/sources/cpp-sdl2/window.hpp
	)

add_library(cpp_sdl2 INTERFACE)
target_compile_features(cpp_sdl2 INTERFACE cxx_std_17)
target_sources(cpp_sdl2 INTERFACE ${CPP_SDL2_SOURCES})
target_include_directories(cpp_sdl2 INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/sources)

find_package(SDL2 CONFIG REQUIRED)
target_link_libraries(cpp_sdl2 INTERFACE SDL2::SDL2)

option(CPP_SDL2_ENABLE_OPENGL "Enable opengl functionalities for windows." OFF)
if(CPP_SDL2_ENABLE_OPENGL)
	find_package(OpenGL REQUIRED)
	target_link_libraries(cpp_sdl2 INTERFACE OpenGL::GL)
	target_compile_definitions(cpp_sdl2 INTERFACE CPP_SDL2_ENABLE_OPENGL)
endif()

option(CPP_SDL2_ENABLE_VULKAN "Enable vulkan functionalities for windows." OFF)
if(CPP_SDL2_ENABLE_VULKAN)
	find_package(Vulkan REQUIRED)
	target_link_libraries(cpp_sdl2 INTERFACE Vulkan::Vulkan)
	target_compile_definitions(cpp_sdl2 INTERFACE CPP_SDL2_ENABLE_VULKAN)
endif()

option(CPP_SDL2_ENABLE_SDL_IMAGE "Use SDL_Image to provide image loading tools" OFF)
if(CPP_SDL2_ENABLE_SDL_IMAGE)
	if(NOT TARGET SDL2::SDL2_image)
		find_package(sdl2-image CONFIG REQUIRED)
	endif()
	target_link_libraries(cpp_sdl2 INTERFACE SDL2::SDL2_image)
	target_compile_definitions(cpp_sdl2 INTERFACE CPP_SDL2_ENABLE_SDL_IMAGE)
endif()

option(CPP_SDL2_DISABLE_EXCEPTIONS "Disable exceptions for cpp-sdl2" OFF)
if(CPP_SDL2_DISABLE_EXCEPTIONS)
	target_compile_definitions(cpp_sdl2 INTERFACE CPP_SDL2_DISABLE_EXCEPTIONS)
endif()

# TODO: handle installation

option(CPP_SDL2_BUILD_EXAMPLES "Build examples" OFF)
if(CPP_SDL2_BUILD_EXAMPLES)
	add_subdirectory(examples)
endif()
