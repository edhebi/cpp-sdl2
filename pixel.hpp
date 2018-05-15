#pragma once

#include <SDL2/SDL_pixels.h>
#include "exception.hpp"
#include "color.hpp"

namespace sdl
{

class Pixel
{
public:
	Pixel(void* target, SDL_PixelFormat const& fmt)
		: target_{ target }
		, fmt_{ fmt } 
	{ }

	Pixel& operator=(Color const& c) { set_color(c); return *this; }
	void set_color(Color const& c)
	{
		set_raw(c.as_uint(fmt_));
	}

	Color color() const	{ return Color{ get_raw(), fmt_ }; }

	Uint8 r() const { return color().r; }
	Uint8 g() const { return color().g; }
	Uint8 b() const { return color().b; }
	Uint8 a() const { return color().a; }

	void set_r(Uint8 r) { auto c = color(); c.r = r; set_color(c); }
	void set_g(Uint8 g) { auto c = color(); c.g = g; set_color(c); }
	void set_b(Uint8 b) { auto c = color(); c.b = b; set_color(c); }
	void set_a(Uint8 a) { auto c = color(); c.a = a; set_color(c); }

private: 
	void set_raw(Uint32 raw)
	{
		switch (fmt_.BytesPerPixel)
		{
		case 1:
			*static_cast<Uint8*>(target_) = static_cast<Uint8>(raw);
			break;
		case 2:
			*static_cast<Uint16*>(target_) = static_cast<Uint16>(raw);
			break;
		case 3:
			if constexpr(SDL_BYTEORDER == SDL_BIG_ENDIAN)
			{
				static_cast<Uint8*>(target_)[0] = static_cast<Uint8>((raw >> 16) & 0xFF);
				static_cast<Uint8*>(target_)[1] = static_cast<Uint8>((raw >> 8)  & 0xFF);
				static_cast<Uint8*>(target_)[2] = static_cast<Uint8>(raw & 0xFF);
			}
			else
			{
				static_cast<Uint8*>(target_)[0] = static_cast<Uint8>(raw & 0xFF);
				static_cast<Uint8*>(target_)[1] = static_cast<Uint8>((raw >> 8)  & 0xFF);
				static_cast<Uint8*>(target_)[2] = static_cast<Uint8>((raw >> 16) & 0xFF);
			}
			break;
		case 4:
			*static_cast<Uint32*>(target_) = raw;
			break;
		}
	}

	Uint32 get_raw() const
	{
		switch (fmt_.BytesPerPixel)
		{
		case 1:
			return *static_cast<Uint8*>(target_);
		case 2:
			return *static_cast<Uint16*>(target_);
		case 3:
			if constexpr(SDL_BYTEORDER == SDL_BIG_ENDIAN)
			{
				return static_cast<Uint8*>(target_)[0] << 16 | static_cast<Uint8*>(target_)[1] << 8 | static_cast<Uint8*>(target_)[2];
			}
			else
			{
				return static_cast<Uint8*>(target_)[0] | static_cast<Uint8*>(target_)[1] << 8 | static_cast<Uint8*>(target_)[2] << 16;
			}
		case 4:
			return *static_cast<Uint32*>(target_);
		}
	}

	void* target_;
	const SDL_PixelFormat& fmt_;
};

} // namespace sdl