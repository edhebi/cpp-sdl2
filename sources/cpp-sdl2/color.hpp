#pragma once

#include "exception.hpp"

#include <SDL_pixels.h>

#include <ostream>

namespace sdl
{
///C++ wrapping around the SDL_Color structure
class Color : public SDL_Color
{
public:
	///Construct an sdl::Color object. Color will be black by default
	constexpr Color() : SDL_Color{0, 0, 0, 255} {}

	///Construct an sdl Color::object with the given 8 bit color values. Alpha channel to fully opaque by default
	/// \param r 8 bit quantity of red
	/// \param g 8 bit quantity of green
	/// \param b 8 bit quantity of blue
	/// \param a 8 bit opacity level. Default to 255 (0xff)
	constexpr Color(Uint8 r, Uint8 g, Uint8 b, Uint8 a = 255)
		: SDL_Color{r, g, b, a}
	{
	}

	///Default copy ctor
	constexpr Color(Color const&) = default;

	///Default move ctor
	constexpr Color(Color&&) = default;

	///Converting constructor that will initialize the color with a given 32 bit value, according to the provided PixelFormat
	Color(Uint32 raw, SDL_PixelFormat const& format)
	{
		SDL_GetRGBA(raw, &format, &r, &g, &b, &a);
	}

	/// \copydoc Color(Uint32 raw, SDL_PixelFormat const& format)
	Color(Uint32 raw, Uint32 format)
	{
		auto f = SDL_AllocFormat(format);
		if (!f) throw Exception{"SDL_AllocFormat"};
		SDL_GetRGBA(raw, f, &r, &g, &b, &a);
		SDL_FreeFormat(f);
	}

	///Default copy assing operator
	Color& operator=(Color const&) = default;

	///Default move assing operator
	Color& operator=(Color&&) = default;

	///Return the color of the current object as a 32bit number (4 bytes)
	/// \param format pixel format to use
	Uint32 as_uint(SDL_PixelFormat const& format) const
	{
		if (SDL_ISPIXELFORMAT_ALPHA(format.format))
		{
			return SDL_MapRGBA(&format, r, g, b, a);
		}
		else
		{
			return SDL_MapRGB(&format, r, g, b);
		}
	}

	/// \copydoc as_uint(SDL_PixelFormat const& format) const
	Uint32 as_uint(Uint32 format) const
	{
		auto f = SDL_AllocFormat(format);
		if (!f) throw Exception{"SDL_AllocFormat"};
		auto raw = as_uint(*f);
		SDL_FreeFormat(f);
		return raw;
	}

	/// Return true if both colors are identical
	bool operator==(Color const& c) const
	{
		return r == c.r && g == c.g && b == c.b && a == c.a;
	}

	/// Output stream overload that will print the value of the current color as a 4D vector of 8bit numbers
	friend std::ostream& operator<<(std::ostream& stream, Color const& c)
	{
		return stream << "(r:" << c.r << ",g:" << c.g << ",b:" << c.b
					  << ",a:" << c.a << ")";
	}

	static constexpr Color White() { return {255, 255, 255, 255}; }
	static constexpr Color Black() { return {0, 0, 0, 255}; }
	static constexpr Color Red() { return {255, 0, 0, 255}; }
	static constexpr Color Green() { return {0, 255, 0, 255}; }
	static constexpr Color Blue() { return {0, 0, 255, 255}; }
	static constexpr Color Transparent() { return {0, 0, 0, 0}; }
};

} // namespace sdl
