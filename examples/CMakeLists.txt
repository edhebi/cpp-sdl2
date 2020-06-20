add_executable(cpp_sdl2_example_general general/main.cpp general/other_unit.cpp)
target_link_libraries(cpp_sdl2_example_general PRIVATE cpp_sdl2 SDL2::SDL2main)

add_library(cpp_sdl2_example_dll_shared SHARED dll/dll.c)
target_link_libraries(cpp_sdl2_example_dll_shared PRIVATE cpp_sdl2)
add_executable(cpp_sdl2_example_dll dll/main.cpp)
target_link_libraries(cpp_sdl2_example_dll PRIVATE cpp_sdl2_example_dll_shared cpp_sdl2 SDL2::SDL2main)

if(CPP_SDL2_ENABLE_OPENGL)
	add_executable(cpp_sdl2_example_opengl opengl/main.cpp opengl/glad/src/glad.c)
	target_include_directories(cpp_sdl2_example_opengl PRIVATE opengl/glad/include)
	target_link_libraries(cpp_sdl2_example_opengl PRIVATE cpp_sdl2 SDL2::SDL2main)
endif()

if(CPP_SDL2_ENABLE_VULKAN)
	add_executable(cpp_sdl2_example_vulkan vulkan/main.cpp)
	target_sources(cpp_sdl2_example_vulkan PRIVATE
		vulkan/simple_tri.frag.glsl
		vulkan/simple_tri.frag.spv
		vulkan/simple_tri.vert.glsl
		vulkan/simple_tri.vert.spv
		)
	target_link_libraries(cpp_sdl2_example_vulkan PRIVATE cpp_sdl2 SDL2::SDL2main)
endif()
