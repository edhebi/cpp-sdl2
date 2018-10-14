#pragma once

#include <string>
#include <utility>

#include <SDL2/SDL_surface.h>

#ifdef CPP_SDL2_USE_SDL_IMAGE
#	include <SDL_image.h>
#endif

#include "color.hpp"
#include "exception.hpp"
#include "pixel.hpp"
#include "rect.hpp"
#include "vec2.hpp"

namespace sdl
{
/// Represent an SDL_Surface
class Surface
{
public:
	/// Represet a lock to safelly access surface content
	class Lock
	{
		friend class Surface;

	public:
		/// Get pixel at given coordinates inside surface
		Pixel at(Vec2i const& pos) const { return at(pos.x, pos.y); }
		/// Get pixel at specified location inside surface
		Pixel at(size_t x, size_t y) const
		{
			return Pixel{static_cast<Uint8*>(surface_->pixels)
							 + (y * surface_->pitch)
							 + (x * surface_->format->BytesPerPixel),
						 *surface_->format};
		}

		/// Get pixel at specified location via operator[]
		Pixel operator[](Vec2i const& pos) const { return at(pos.x, pos.y); }

		/// Get access to the raw array of pixels
		void* raw_array() const { return surface_->pixels; }

		/// Free the lock
		~Lock() { SDL_UnlockSurface(surface_); }

	private:
		/// Private constructor for lock object.
		constexpr Lock(SDL_Surface& surface_) : surface_{&surface_} {}

		/// Raw pointer to the surface
		SDL_Surface* surface_;
	};

	/// Explicit converting ctor from C SDL_Surface to sdl::Surface
	explicit Surface(SDL_Surface* surface) : surface_{surface} {}

	/// Use default move ctor
	Surface(Surface&&) noexcept = default;
	/// Use defautl move assing operator
	Surface& operator=(Surface&& other)
	{
		SDL_FreeSurface(surface_);
		surface_ = std::move(other.surface_);
		return *this;
	}

	/// Create surface for given parameters
	Surface(
		Uint32 flags,
		int	w,
		int	h,
		int	depth,
		Uint32 rmask,
		Uint32 gmask,
		Uint32 bmask,
		Uint32 amask)
		: surface_{SDL_CreateRGBSurface(
			  flags, w, h, depth, rmask, gmask, bmask, amask)}
	{
		if (!surface_) throw Exception{"SDL_CreateRGBSurface"};
	}

	/// Create surface from array of pixels
	Surface(
		void*  pixels,
		int	w,
		int	h,
		int	depth,
		int	pitch,
		Uint32 rmask,
		Uint32 gmask,
		Uint32 bmask,
		Uint32 amask)
		: surface_{SDL_CreateRGBSurfaceFrom(
			  pixels, w, h, depth, pitch, rmask, gmask, bmask, amask)}
	{
		if (!surface_) throw Exception{"SDL_CreateRGBSurfaceFrom"};
	}

	/// Create surface with specified format
	Surface(Uint32 flags, int w, int h, int depth, Uint32 format)
		: surface_{SDL_CreateRGBSurfaceWithFormat(flags, w, h, depth, format)}
	{
		if (!surface_) throw Exception{"SDL_CreateRGBSurfaceWithFormat"};
	}

	/// Create surface from array of pixels
	Surface(void* pixels, int w, int h, int depth, int pitch, int format)
		: surface_{SDL_CreateRGBSurfaceWithFormatFrom(
			  pixels, w, h, depth, pitch, format)}
	{
		if (!surface_) throw Exception{"SDL_CreateRGBSurfaceWithFormatFrom"};
	}

#ifdef CPP_SDL2_USE_SDL_IMAGE
	/// Construct surface from image file, require SDL_Image
	Surface(std::string const& filename) : surface_{IMG_Load(filename.c_str())}
	{
		if (!surface_) throw Exception{"IMG_Load"};
	}

#else
	/// Stubbed constructor for missing SDL_Image usage.
	Surface(std::string const& filename) : surface_{nullptr}
	{
		SDL_SetError(
			"Tried to call sdl::Surface(std::string const& filename) ctor. "
			"This function should call IMG_Load() from SDL_Image.\n"
			"This program was built without SDL_Image.\n"
			"Please Install SDL_Image and #define CPP_SDL2_USE_SDL_IMAGE "
			"before including sdl.hpp to use this functionality");
		throw Exception("IMG_Load");
	}

#endif

	/// RAII dtor to automatically free the surface
	~Surface() { SDL_FreeSurface(surface_); }

	/// Get C SDL_Surface objetct
	SDL_Surface* ptr() const { return surface_; }

	/// Convert surface to given format
	///\param format What pixel format to use
	Surface with_format(SDL_PixelFormat const& format) const
	{
		auto s = SDL_ConvertSurface(surface_, &format, 0);
		if (!s) throw Exception{"SDL_ConvertSurface"};
		return Surface{s};
	}

	/// convert surface from given format
	///\param format what format to use
	Surface with_format(Uint32 format) const
	{
		auto s = SDL_ConvertSurfaceFormat(surface_, format, 0);
		if (!s) throw Exception{"SDL_ConvertSurfaceFormat"};
		return Surface{s};
	}

	/// Convert this surface to specified format
	Surface& convert_to(SDL_PixelFormat const& format)
	{
		return *this = with_format(format);
	}
	/// Convert this surface to specified format
	Surface& convert_to(Uint32 format) { return *this = with_format(format); }

	/// Blit surface on another
	void blit_on(Rect const& src, Surface& surf, Rect const& dst) const
	{
		// dst rectangle will *not* be modified by a blit
		auto dstmut = const_cast<Rect&>(dst);
		if (SDL_BlitSurface(surface_, &src, surf.surface_, &dstmut) != 0)
		{
			throw Exception{"SDL_BlitSurface"};
		}
	}

	/// Blit surface on another
	void blit_on(Surface& surf, Rect const& dst) const
	{ // dst rectangle will *not* be modified by a blit
		auto dstmut = const_cast<Rect&>(dst);
		if (SDL_BlitSurface(surface_, nullptr, surf.surface_, &dstmut) != 0)
		{
			throw Exception{"SDL_BlitSurface"};
		}
	}

	/// Get width of surface
	int width() const { return surface_->w; }
	/// Get height of surface
	int height() const { return surface_->h; }
	/// Get size of surface
	Vec2i size() const { return Vec2i{width(), height()}; }

	/// Get surface's pixel format
	SDL_PixelFormat const& pixelformat() const { return *surface_->format; }

	/// Get surface format
	Uint32 format() const { return surface_->format->format; }

	/// Get surface flags
	Uint32 flags() const { return surface_->flags; }

	/// Get surface clip rectangle
	Rect cliprect() const
	{
		Rect r;
		SDL_GetClipRect(surface_, &r);
		return r;
	}

	/// disable colorkey
	void disable_colorkey()
	{
		if (SDL_SetColorKey(surface_, SDL_FALSE, 0) != 0)
			throw Exception{"SDL_SetColorKey"};
	}

	/// Set colorkey
	void set_colorkey(Uint32 key)
	{
		if (SDL_SetColorKey(surface_, SDL_TRUE, key) != 0)
			throw Exception{"SDL_SetColorKey"};
	}
	/// Set colorkey
	void set_colorkey(Color const& color)
	{
		if (SDL_SetColorKey(surface_, SDL_TRUE, color.as_uint(pixelformat()))
			!= 0)
		{
			throw Exception{"SDL_SetColorKey"};
		}
	}

	/// Get color key
	Color colorkey() const
	{
		Uint32 k;
		if (SDL_GetColorKey(surface_, &k) != 0)
			throw Exception{"SDL_GetColorKey"};
		return Color{k, pixelformat()};
	}

	/// Set surface blend mode
	void set_blendmode(SDL_BlendMode const& bm)
	{
		if (SDL_SetSurfaceBlendMode(surface_, bm) != 0)
			throw Exception{"SDL_SetSurfaceBlendMode"};
	}
	SDL_BlendMode blendmode() const
	{
		SDL_BlendMode bm;
		if (SDL_GetSurfaceBlendMode(surface_, &bm) != 0)
			throw Exception{"SDL_GetSurfaceBlendMode"};
		return bm;
	}

	/// Set colormod
	void set_colormod(Color const& color)
	{
		set_colormod(color.r, color.g, color.b);
	}
	/// Set colormod
	void set_colormod(Uint8 r, Uint8 g, Uint8 b)
	{
		if (SDL_SetSurfaceColorMod(surface_, r, g, b))
			throw Exception{"SDL_SetSurfaceColorMod"};
	}

	/// Get colormod
	Color colormod() const
	{
		Color c;
		if (SDL_GetSurfaceColorMod(surface_, &c.r, &c.g, &c.b) != 0)
			throw Exception{"SDL_SetSurfaceColorMod"};
		return c;
	}

	/// Set alphamod
	void set_alphamod(Uint8 alpha)
	{
		if (SDL_SetSurfaceAlphaMod(surface_, alpha) != 0)
			throw Exception{"SDL_SetSurfaceAlphaMod"};
	}
	/// Get alphamod
	Uint8 alphamod() const
	{
		Uint8 alpha;
		if (SDL_GetSurfaceAlphaMod(surface_, &alpha) != 0)
			throw Exception{"SDL_GetSurfaceAlphaMod"};
		return alpha;
	}

	/// Set color and alpha mod
	void set_coloralphamod(Uint8 r, Uint8 g, Uint8 b, Uint8 a)
	{
		set_colormod(r, g, b);
		set_alphamod(a);
	}
	/// Set color and alpha mod
	void set_coloralphamod(Color const& c)
	{
		set_colormod(c.r, c.g, c.b);
		set_alphamod(c.a);
	}
	/// Get color and alpha mod
	Color coloralphamod()
	{
		auto c = colormod();
		c.a	= alphamod();
		return c;
	}

	/// Lock surface for raw access to pixel. Return a lock object that permit
	/// to access the pixels. When lock goes out of scope, resource goes unlocked
	/// automatically
	[[nodiscard]] Lock lock()
	{
		if (SDL_LockSurface(surface_) != 0) throw Exception{"SDL_LockSurface"};
		return Lock{*surface_};
	}

private:
	/// Set surface pointer
	SDL_Surface* surface_;
};

} // namespace sdl
