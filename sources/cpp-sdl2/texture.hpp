#pragma once

#include "color.hpp"
#include "exception.hpp"
#include "pixel.hpp"
#include "rect.hpp"
#include "surface.hpp"
#include "vec2.hpp"

#include <SDL_render.h>

#include <string>

namespace sdl
{
///Class that represet a renderer texture
class Texture
{
public:
	///Lock object that permit to access pixels directly on texture
	class Lock
	{
		friend class Texture;

	public:
		///Get pixel at given location on texture
		Pixel at(Vec2i const& pos) const { return at(pos.x, pos.y); }
		///Get pixel at given location on texture
		Pixel at(size_t x, size_t y) const
		{
			return Pixel{static_cast<Uint8*>(pixels_) + (y * pitch_)
							 + (x * format_->BytesPerPixel),
						 *format_};
		}

		///Get pixel at location with operator[]
		Pixel operator[](Vec2i const& pos) const { return at(pos.x, pos.y); }

		///Automatically unlock texture
		~Lock()
		{
			SDL_UnlockTexture(texture_);
			SDL_FreeFormat(format_);
		}

	private:
		///private ctor to create a lock. Lock are created by Texture class
		Lock(SDL_Texture* texture, SDL_Rect const* rect) : texture_{texture}
		{
			if (SDL_LockTexture(texture_, rect, &pixels_, &pitch_) != 0)
			{
				throw Exception{"SDL_LockTexture"};
			}

			Uint32 f;
			SDL_QueryTexture(texture_, &f, nullptr, nullptr, nullptr);
			format_ = SDL_AllocFormat(f);

			if (!format_) throw Exception{"SDL_AllocFormat"};
		}

		///pointer to raw texure
		SDL_Texture* texture_;
		///pointer to pixel array
		void* pixels_;
		///Pixel pitch
		int pitch_;
		///Pixel format
		SDL_PixelFormat* format_;
	};

	///Construct texture from C SDL_Texture object
	explicit Texture(SDL_Texture* t) : texture_{t} {};

	///Get SDL_Texture pointer from texture
	SDL_Texture* ptr() const { return texture_; }

	///Create texture
	Texture(
		SDL_Renderer*	 render,
		Uint32			  format,
		SDL_TextureAccess access,
		int				  w,
		int				  h)
		: Texture{SDL_CreateTexture(render, format, access, w, h)}
	{
		if (!texture_) throw Exception{"SDL_CreateTexture"};
	}

	///Default constructor for empty texture object
	Texture() = default;
	
	///Create texture
	Texture(
		SDL_Renderer*	 render,
		Uint32			  format,
		SDL_TextureAccess access,
		Vec2i			  size)
		: Texture{render, format, access, size.x, size.y}
	{
	}

	///Create texture from surface
	Texture(SDL_Renderer* render, Surface const& surface)
		: Texture{SDL_CreateTextureFromSurface(render, surface.ptr())}
	{
		if (!texture_) throw Exception{"SDL_CreateTextureFromSurface"};
	}

	///Create texture from file
	Texture(SDL_Renderer* render, std::string const& filename)
		: Texture{render, Surface{filename}}
	{
	}

	///Move texture into this one
	Texture(Texture&& other) noexcept { *this = std::move(other);
	}

	///move texture into this one
	Texture& operator=(Texture&& other) noexcept
	{
		if (texture_ != other.texture_)
		{
			SDL_DestroyTexture(texture_);
			texture_= other.texture_;
			other.texture_ = nullptr;
			return *this;
		}
	}

	///Non copiable
	Texture(Texture const&) = delete;
	///Non copiable
	Texture& operator=(Texture const&) = delete;

	///Destroy texture when it's not in use anymore
	~Texture() { SDL_DestroyTexture(texture_); }

	///Set texture blend mode
	void set_blendmode(SDL_BlendMode const& bm) const
	{
		if (SDL_SetTextureBlendMode(texture_, bm) != 0)
			throw Exception{"SDL_SetTextureBlendMode"};
	}
	///Get texture blend mode
	SDL_BlendMode blendmode() const
	{
		SDL_BlendMode bm;
		if (SDL_GetTextureBlendMode(texture_, &bm) != 0)
			throw Exception{"SDL_GetTextureBlendMode"};
		return bm;
	}

	///Set colormod
	void set_colormod(Color const& color) const
	{
		set_colormod(color.r, color.g, color.b);
	}
	///Set colormod
	void set_colormod(Uint8 r, Uint8 g, Uint8 b) const
	{
		if (SDL_SetTextureColorMod(texture_, r, g, b) != 0)
			throw Exception{"SDL_SetTextureColorMod"};
	}

	///Get colormod
	Color colormod() const
	{
		Color c;
		if (SDL_GetTextureColorMod(texture_, &c.r, &c.g, &c.b) != 0)
			throw Exception{"SDL_SetTextureColorMod"};
		return c;
	}

	///Set alphamod
	void set_alphamod(Uint8 alpha) const
	{
		if (SDL_SetTextureAlphaMod(texture_, alpha) != 0)
			throw Exception{"SDL_SetTextureAlphaMod"};
	}
	///Set alphamod
	Uint8 alphamod() const
	{
		Uint8 alpha;
		if (SDL_GetTextureAlphaMod(texture_, &alpha) != 0)
			throw Exception{"SDL_GetTextureAlphaMod"};
		return alpha;
	}

	///Set coloralphamod
	void set_coloralphamod(Uint8 r, Uint8 g, Uint8 b, Uint8 a) const
	{
		set_colormod(r, g, b);
		set_alphamod(a);
	}
	///Set coloralphamod
	void set_coloralphamod(Color const& c) const
	{
		set_colormod(c);
		set_alphamod(c.a);
	}
	///Get coloralphamod
	Color coloralphamod() const
	{
		auto c = colormod();
		c.a	= alphamod();
		return c;
	}

	///Get texture format
	Uint32 format() const
	{
		Uint32 f;
		if (SDL_QueryTexture(texture_, &f, nullptr, nullptr, nullptr) != 0)
			throw Exception{"SDL_QueryTexture"};
		return f;
	}

	///Access texture
	int access() const
	{
		int a;
		if (SDL_QueryTexture(texture_, nullptr, &a, nullptr, nullptr) != 0)
			throw Exception{"SDL_QueryTexture"};
		return a;
	}

	///Get texture size
	Vec2i size() const
	{
		Vec2i s;
		if (SDL_QueryTexture(texture_, nullptr, nullptr, &s.x, &s.y) != 0)
			throw Exception{"SDL_QueryTexture"};
		return s;
	}

	///lock texture for direct access to content
	[[nodiscard]] Lock lock() { return Lock{texture_, nullptr}; }

	///lock texture rect
	[[nodiscard]] Lock lock(Rect const& rect) { return Lock{texture_, &rect}; }

private:
	SDL_Texture* texture_ = nullptr;
};

} // namespace sdl
