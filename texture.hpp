#pragma once

#include <string>
#include <SDL2/SDL_render.h>

#include "color.hpp"
#include "exception.hpp"
#include "surface.hpp"
#include "vec2.hpp"
#include "rect.hpp"
#include "pixel.hpp"

namespace sdl
{

class Texture
{
public:

	class Lock
	{
		friend class Texture;

	public:
		Pixel at(Vec2i const& pos) const { return at(pos.x, pos.y); }
		Pixel at(size_t x, size_t y) const
		{
			return Pixel{ static_cast<Uint8*>(pixels_) + (y * pitch_) + (x * format_->BytesPerPixel), *format_ };
		}

		Pixel operator[](Vec2i const& pos) const { return at(pos.x, pos.y); }

		~Lock() { SDL_UnlockTexture(texture_); SDL_FreeFormat(format_); }

	private:
		
		Lock(SDL_Texture* texture, SDL_Rect const* rect) : texture_{ texture }
		{
			if (SDL_LockTexture(texture_, rect, &pixels_, &pitch_) != 0)
			{
				throw Exception{ "SDL_LockTexture" };
			}

			Uint32 f;
			SDL_QueryTexture(texture_, &f, nullptr, nullptr, nullptr);
			format_ = SDL_AllocFormat(f);

			if (!format_) throw Exception{ "SDL_AllocFormat" };
		}

		SDL_Texture* texture_;
		void* pixels_;
		int pitch_;
		SDL_PixelFormat* format_;
	};


	explicit Texture(SDL_Texture* t) : texture_{ t } {};

	Texture(SDL_Renderer* render, Uint32 format, SDL_TextureAccess access, int w, int h)
		: Texture{ SDL_CreateTexture(render, format, access, w, h) }
	{
		if (!texture_) throw Exception{ "SDL_CreateTexture" };
	}

	Texture(SDL_Renderer* render, Uint32 format, SDL_TextureAccess access, Vec2i size)
		: Texture{ render, format, access, size.x, size.y }
	{
	}

	Texture(SDL_Renderer* render, Surface const& surface)
		: Texture{ SDL_CreateTextureFromSurface(render, surface.ptr()) }
	{
		if (!texture_) throw Exception{ "SDL_CreateTextureFromSurface" };
	}

	Texture(SDL_Renderer* render, std::string const& filename)
		: Texture{ render, Surface{ filename } }
	{
	}

	Texture(Texture&& other) : texture_{ other.texture_ }
	{
		other.texture_ = nullptr;
	}
	
	Texture& operator=(Texture&& other)
	{
		SDL_DestroyTexture(texture_);
		texture_ = other.texture_;
		other.texture_ = nullptr;
		return *this;
	}

	Texture(Texture const&) = delete;
	Texture& operator=(Texture const&) = delete;

	~Texture() { SDL_DestroyTexture(texture_); }

	void set_blendmode(SDL_BlendMode const& bm) { if (SDL_SetTextureBlendMode(texture_, bm) != 0) throw Exception{ "SDL_SetTextureBlendMode" }; }
	SDL_BlendMode blendmode() const
	{
		SDL_BlendMode bm;
		if (SDL_GetTextureBlendMode(texture_, &bm) != 0) throw Exception{ "SDL_GetTextureBlendMode" };
		return bm;
	}

	void set_colormod(Color const& color) { set_colormod(color.r, color.g, color.b); }
	void set_colormod(Uint8 r, Uint8 g, Uint8 b)
	{
		if (SDL_SetTextureColorMod(texture_, r, g, b) != 0) throw Exception{ "SDL_SetTextureColorMod" };
	}

	Color colormod() const
	{
		Color c;
		if (SDL_GetTextureColorMod(texture_, &c.r, &c.g, &c.b) != 0) throw Exception{ "SDL_SetTextureColorMod" };
		return c;
	}

	void set_alphamod(Uint8 alpha) { if (SDL_SetTextureAlphaMod(texture_, alpha) != 0) throw Exception{ "SDL_SetTextureAlphaMod" }; }
	Uint8 alphamod() const
	{
		Uint8 alpha;
		if (SDL_GetTextureAlphaMod(texture_, &alpha) != 0) throw Exception{ "SDL_GetTextureAlphaMod" };
		return alpha;
	}

	void set_coloralphamod(Uint8 r, Uint8 g, Uint8 b, Uint8 a) { set_colormod(r, g, b); set_alphamod(a); }
	void set_coloralphamod(Color const& c) { set_colormod(c); set_alphamod(c.a); }
	Color coloralphamod() { auto c = colormod(); c.a = alphamod(); return c; }

	Uint32 format() const
	{
		Uint32 f;
		if (SDL_QueryTexture(texture_, &f, nullptr, nullptr, nullptr) != 0) throw Exception{ "SDL_QueryTexture" };
		return f;
	}

	int access()
	{
		int a;
		if (SDL_QueryTexture(texture_, nullptr, &a, nullptr, nullptr) != 0) throw Exception{ "SDL_QueryTexture" };
		return a;
	}

	Vec2i size()
	{
		Vec2i s;
		if (SDL_QueryTexture(texture_, nullptr, nullptr, &s.x, &s.y) != 0) throw Exception{ "SDL_QueryTexture" };
		return s;
	}


	[[nodiscard]] Lock lock()
	{
		return Lock{ texture_, nullptr };
	}

	[[nodiscard]] Lock lock(Rect const& rect)
	{
		return Lock{ texture_, &rect };
	}

private:

	SDL_Texture* texture_;
};

} // namespace sdl
