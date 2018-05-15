#pragma once

#include <SDL2/SDL.h>

#include "color.hpp"
#include "event.hpp"
#include "exception.hpp"
#include "rect.hpp"
#include "renderer.hpp"
#include "surface.hpp"
#include "texture.hpp"
#include "vec2.hpp"
#include "window.hpp"

namespace sdl
{

class [[nodiscard]] Root
{
public:
	Root(Uint32 flags) { if (SDL_Init(flags) != 0) throw Exception{ "SDL_Init" }; }

	~Root() { SDL_Quit(); }
};

} // namespace sdl