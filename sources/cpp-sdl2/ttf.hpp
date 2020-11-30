#pragma once
#ifdef CPP_SDL2_ENABLE_SDL_TTF
#include <SDL_ttf.h>
#include <SDL_surface.h>
#include <SDL_pixels.h>
#include <cpp-sdl2/surface.hpp>

namespace sdl
{
///Class that represents a SDL_ttf font
class TTF
{
public:
	enum class Style {
		normal=0x00,
		bold=0x01,
		italic=0x02,
		underline=0x04,
		strikethrough=0x08
	};

	enum class Hinting {
		normal=0,
		light=1,
		mono=2,
		none=3
	};

	///Construct a font from the TTF_Font C object
	explicit TTF(TTF_Font* font) : font_{font} {}

	///Default ctor, create an empty font object
	TTF() {}

	///Default ctor, create an empty font object
	TTF(const char* font, size_t size): font_(TTF_OpenFont(font, size))
	{
		if (!font_) throw Exception{"SDL_TTF_OpenFont"};
	}

	///Default move ctor
	TTF(TTF&& other) noexcept { *this = std::move(other); }

	///Move a font to this one
	TTF& operator=(TTF&& other) noexcept
	{
		if (font_ != other.font_)
		{
			TTF_CloseFont(font_);
			font_		= other.font_;
			other.font_ = nullptr;
		}
		return *this;
	}

	///Destroy the font automaticlally when object goes out of scope
	~TTF() { TTF_CloseFont(font_); }

	///This is a managed RAII resource. this object is not copyable
	TTF(TTF const&) = delete;

	///This is a managed RAII resource. this object is not copyable
	TTF& operator=(TTF const&) = delete;

	///Return a pointer to the wrapped C TTF_Font
	TTF_Font* ptr() const { return font_; }

	sdl::Surface renderUTF8Solid(const char* text, sdl::Color fg = {0, 0, 0}) const
	{
		return sdl::Surface(TTF_RenderUTF8_Solid(font_, text, fg));
	}

	sdl::Surface renderUTF8Blended(const char* text, sdl::Color fg = {0, 0, 0}) const
	{
		return sdl::Surface(TTF_RenderUTF8_Blended(font_, text, fg));
	}

	static void Init()
	{
		TTF_Init();
	}
private:
	///Pointer to raw TTF_Font
	TTF_Font* font_ = nullptr;
};
};
#endif