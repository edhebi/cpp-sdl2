# cpp-sdl2 OpenGL example

This is a small "hello opengl" program, but that uses cpp-sdl2 for windowing and context management.

the interesting code for our demo amont to this : 

Include like this (with an OpenGL loader of your choice)
```cpp
// To use the GL wrapper, define the following
#define CPP_SDL2_GL_WINDOW
#include <cpp-sdl2/sdl.hpp>
#include <glad/glad.h>
```

//Begining of `main()`
```cpp

	// Create an SDL window, with the SDL_WINDOW_OPENGL flags
	auto window = sdl::Window("OpenGL", {800, 600}, SDL_WINDOW_OPENGL);

	// Bevore creating a context, set the flag for the version you want to get,
	// here we want Core OpenGL 3.3
	sdl::Window::gl_set_attribute(
		SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	sdl::Window::gl_set_attribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	sdl::Window::gl_set_attribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

	// Create your context
	auto context = window.create_context();

	// You are done, now you can use OpenGL!

	// Call whatever function loader you want, in this example we use GLAD
	// because we generated a really small version of it for 3.3 Core:
	gladLoadGL();

```
