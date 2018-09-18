#pragma once

#include <utility>
#include <string>

#include <SDL2/SDL_surface.h>

#ifdef CPP_SDL2_USE_SDL_IMAGE
#include <SDL_image.h>
#endif

#include "exception.hpp"
#include "vec2.hpp"
#include "rect.hpp"
#include "color.hpp"
#include "pixel.hpp"

namespace sdl
{

class Surface
{
public:

	class Lock
	{
		friend class Surface;

	public:

		Pixel at(Vec2i const& pos) const { return at(pos.x, pos.y); }
		Pixel at(size_t x, size_t y) const
		{
			return Pixel{ static_cast<Uint8*>(surface_->pixels) + (y * surface_->pitch) + (x * surface_->format->BytesPerPixel), *surface_->format };
		}

		Pixel operator[](Vec2i const& pos) const { return at(pos.x, pos.y); }

		void* raw_array() const { return surface_->pixels; }

		~Lock() { SDL_UnlockSurface(surface_); }

	private:
		constexpr Lock(SDL_Surface& surface_) : surface_{ &surface_ } {}

		SDL_Surface* surface_;
	};

	explicit Surface(SDL_Surface* surface) : surface_{ surface } {}
	
	Surface(Surface&&) noexcept = default;
	Surface& operator=(Surface&& other) { SDL_FreeSurface(surface_); surface_ = std::move(other.surface_); return *this; }

	Surface(Uint32 flags, int w, int h, int depth, Uint32 rmask, Uint32 gmask, Uint32 bmask, Uint32 amask)
		: surface_{ SDL_CreateRGBSurface(flags, w, h, depth, rmask, gmask, bmask, amask) }
	{
		if (!surface_) throw Exception{ "SDL_CreateRGBSurface" };
	}

	Surface(void* pixels, int w, int h, int depth, int pitch, Uint32 rmask, Uint32 gmask, Uint32 bmask, Uint32 amask)
		: surface_{ SDL_CreateRGBSurfaceFrom(pixels, w, h, depth, pitch, rmask, gmask, bmask, amask) }
	{
		if (!surface_) throw Exception{ "SDL_CreateRGBSurfaceFrom" };
	}


	Surface(Uint32 flags, int w, int h, int depth, Uint32 format)
		: surface_{ SDL_CreateRGBSurfaceWithFormat(flags, w, h, depth, format) }
	{
		if (!surface_) throw Exception{ "SDL_CreateRGBSurfaceWithFormat" };
	}

	Surface(void* pixels, int w, int h, int depth, int pitch, int format)
		: surface_{ SDL_CreateRGBSurfaceWithFormatFrom(pixels, w, h, depth, pitch, format) }
	{
		if (!surface_) throw Exception{ "SDL_CreateRGBSurfaceWithFormatFrom" };
	}

#ifdef CPP_SDL2_USE_SDL_IMAGE
	Surface(std::string const& filename)
		: surface_{ IMG_Load(filename.c_str()) }
	{
		if (!surface_) throw Exception{ "IMG_Load" };
	}

#else
	Surface(std::string const& filename)
		: surface_{nullptr}
	{
		SDL_SetError( "Tried to call sdl::Surface(std::string const& filename) ctor. This function should call IMG_Load() from SDL_Image.\n"
			"This program was built without SDL_Image.\n"
			"Please Install SDL_Image and #define CPP_SDL2_USE_SDL_IMAGE before including sdl.hpp to use this functionality" );
		throw Exception("IMG_Load");
	}

#endif

	~Surface() { SDL_FreeSurface(surface_); }

	SDL_Surface* ptr() const { return surface_; }

	Surface with_format(SDL_PixelFormat const& format) const
	{
		auto s = SDL_ConvertSurface(surface_, &format, 0);
		if (!s) throw Exception{ "SDL_ConvertSurface" };
		return Surface{ s };
	}

	Surface with_format(Uint32 format) const
	{
		auto s = SDL_ConvertSurfaceFormat(surface_, format, 0);
		if (!s) throw Exception{ "SDL_ConvertSurfaceFormat" };
		return Surface{ s };
	}

	Surface& convert_to(SDL_PixelFormat const& format) { return *this = with_format(format); }
	Surface& convert_to(Uint32 format)                 { return *this = with_format(format); }

	void blit_on(Rect const& src, Surface& surf, Rect const& dst) const
	{
		// dst rectangle will *not* be modified by a blit
		auto dstmut = const_cast<Rect&>(dst);
		if (SDL_BlitSurface(surface_, &src, surf.surface_, &dstmut) != 0)
		{
			throw Exception{ "SDL_BlitSurface" };
		}
	}

	void blit_on(Surface& surf, Rect const& dst) const
	{		// dst rectangle will *not* be modified by a blit
		auto dstmut = const_cast<Rect&>(dst);
		if (SDL_BlitSurface(surface_, nullptr, surf.surface_, &dstmut) != 0)
		{
			throw Exception{ "SDL_BlitSurface" };
		}
	}

	int width()  const { return surface_->w; }
	int height() const { return surface_->h; }
	Vec2i size() const { return Vec2i{ width(), height() }; }
	
	SDL_PixelFormat const& pixelformat() const { return *surface_->format; }
	Uint32 format() const { return surface_->format->format; }
	
	Uint32 flags() const { return surface_->flags; }
	
	Rect cliprect() const { Rect r; SDL_GetClipRect(surface_, &r); return r; }

	void disable_colorkey() { if (SDL_SetColorKey(surface_, SDL_FALSE, 0) != 0) throw Exception{ "SDL_SetColorKey" }; }
	
	void set_colorkey(Uint32 key) { if (SDL_SetColorKey(surface_, SDL_TRUE, key) != 0) throw Exception{ "SDL_SetColorKey" }; }
	void set_colorkey(Color const& color)
	{
		if (SDL_SetColorKey(surface_, SDL_TRUE, color.as_uint(pixelformat())) != 0)
		{
			throw Exception{ "SDL_SetColorKey" };
		}
	}
	
	Color colorkey() const 
	{ 
		Uint32 k;
		if (SDL_GetColorKey(surface_, &k) != 0) throw Exception{ "SDL_GetColorKey" };
		return Color{ k, pixelformat() };
	}

	void set_blendmode(SDL_BlendMode const& bm) { if (SDL_SetSurfaceBlendMode(surface_, bm) != 0) throw Exception{ "SDL_SetSurfaceBlendMode" }; }
	SDL_BlendMode blendmode() const
	{ 
		SDL_BlendMode bm;
		if (SDL_GetSurfaceBlendMode(surface_, &bm) != 0) throw Exception{ "SDL_GetSurfaceBlendMode" };
		return bm;
	}

	void set_colormod(Color const& color) { set_colormod(color.r, color.g, color.b); }
	void set_colormod(Uint8 r, Uint8 g, Uint8 b)
	{
		if (SDL_SetSurfaceColorMod(surface_, r, g, b)) throw Exception{ "SDL_SetSurfaceColorMod" };
	}

	Color colormod() const 
	{
		Color c;
		if (SDL_GetSurfaceColorMod(surface_, &c.r, &c.g, &c.b) != 0) throw Exception{ "SDL_SetSurfaceColorMod" };
		return c;
	}

	void set_alphamod(Uint8 alpha) { if (SDL_SetSurfaceAlphaMod(surface_, alpha) != 0) throw Exception{ "SDL_SetSurfaceAlphaMod" }; }
	Uint8 alphamod() const
	{
		Uint8 alpha;
		if (SDL_GetSurfaceAlphaMod(surface_, &alpha) != 0) throw Exception{ "SDL_GetSurfaceAlphaMod" };
		return alpha;
	}

	void set_coloralphamod(Uint8 r, Uint8 g, Uint8 b, Uint8 a) { set_colormod(r, g, b); set_alphamod(a); }
	void set_coloralphamod(Color const& c) { set_colormod(c.r, c.g, c.b); set_alphamod(c.a); }
	Color coloralphamod() { auto c = colormod(); c.a = alphamod(); return c; }

	[[nodiscard]] Lock lock()
	{
		if (SDL_LockSurface(surface_) != 0) throw Exception{ "SDL_LockSurface" };
		return Lock{ *surface_ };
	}

private:
	SDL_Surface* surface_;
};

} // namespace sdl
