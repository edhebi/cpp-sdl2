 #pragma once

#include <utility>
#include <vector>
#include <SDL2/SDL.h>
#include <SDL2/SDL_render.h>

#include "color.hpp"
#include "exception.hpp"
#include "texture.hpp"
#include "surface.hpp"
#include "rect.hpp"


namespace sdl
{

class Renderer
{
public:

	explicit Renderer(SDL_Renderer* renderer) : renderer_{ renderer } {}
	Renderer(Renderer&& other) = default;

	Renderer& operator=(Renderer&& other)
	{
		SDL_DestroyRenderer(renderer_);
		renderer_ = other.renderer_;
		other.renderer_ = nullptr;
		return *this;
	}
	
	~Renderer() { SDL_DestroyRenderer(renderer_); }

	Renderer(Renderer const&) = delete;
	Renderer& operator=(Renderer const&) = delete;

	SDL_Renderer* ptr() { return renderer_; }

	////////////////
	// Attributes //
	////////////////

	void get_info(SDL_RendererInfo& info) { if (SDL_GetRendererInfo(renderer_, &info) != 0) throw Exception{ "SDL_GetRendererInfo" }; }
	SDL_RendererInfo info() { SDL_RendererInfo info; get_info(info); return info; }

	Color drawcolor()
	{
		Color c;
		if (SDL_GetRenderDrawColor(renderer_, &c.r, &c.g, &c.b, &c.a) != 0) throw Exception{ "SDL_GetRenderDrawColor" };
		return c;
	}

	void set_drawcolor(Uint8 r, Uint8 g, Uint8 b, Uint8 a = SDL_ALPHA_OPAQUE)
	{
		if (SDL_SetRenderDrawColor(renderer_, r, g, b, a) != 0) throw Exception{ "SDL_SetRenderDrawColor" };
	}

	void set_drawcolor(Color const& c) { set_drawcolor(c.r, c.g, c.b, c.a); }

	Rect cliprect() { Rect r; SDL_RenderGetClipRect(renderer_, &r); return r; }
	void set_cliprect(Rect const& r) { if (SDL_RenderSetClipRect(renderer_, &r) != 0) throw Exception{ "SDL_RenderSetClipRect" }; }
	
	bool clip_enabled() { return SDL_RenderIsClipEnabled(renderer_); }
	void disable_clip() { if (SDL_RenderSetClipRect(renderer_, nullptr) != 0) throw Exception{ "SDL_RenderSetClipRect" }; }

	bool intscale() { return SDL_RenderGetIntegerScale(renderer_); }
	void set_intscale(bool intscale) { if (SDL_RenderSetIntegerScale(renderer_, SDL_bool(intscale)) != 0) throw Exception{ "SDL_RenderSetIntegerScale" }; }

	////////////////////
	// TEXTURE MAKING //
	////////////////////

	Texture make_texture(Uint32 format, SDL_TextureAccess access, int w, int h) const
	{
		return Texture{ renderer_, format, access, w, h }; 
	}

	Texture make_texture(Uint32 format, SDL_TextureAccess access, Vec2i size) const 
	{ 
		return Texture{ renderer_, format, access, size.x, size.y }; 
	}
	
	Texture make_texture(Surface const& surface) const
	{
		return Texture{ renderer_, surface }; 
	}
	
	Texture make_texture(std::string const& filename) const
	{
		return Texture{ renderer_, filename }; 
	}

	/////////////
	// DRAWING //
	/////////////

	Renderer& present() { SDL_RenderPresent(renderer_); return *this; }

	void clear() { if (SDL_RenderClear(renderer_) != 0) throw Exception{ "SDL_RenderClear" }; }
	void clear(Color const& c) { set_drawcolor(c); clear(); }

	void draw_line(Vec2i const& pos1, Vec2i const& pos2) { if (SDL_RenderDrawLine(renderer_, pos1.x, pos1.y, pos2.x, pos2.y) != 0) throw Exception{ "SDL_RenderDrawLine" }; }
	void draw_line(Vec2i const& pos1, Vec2i const& pos2, Color const& c) { set_drawcolor(c); draw_line(pos1, pos2); }


	void draw_lines(std::vector<Vec2i> const& points) { if (SDL_RenderDrawLines(renderer_,  &points[0], (int)points.size()) != 0) throw Exception{ "SDL_RenderDrawLines" }; }
	void draw_lines(std::vector<Vec2i> const& points, Color const& c) { set_drawcolor(c); draw_lines(points); }

	void draw_point(Vec2i const& point) { if (SDL_RenderDrawPoint(renderer_, point.x, point.y) != 0) throw Exception{ "SDL_RenderDrawPoint" }; }
	void draw_point(Vec2i const& point, Color const& c) { set_drawcolor(c); draw_point(point);; }

	void draw_points(std::vector<Vec2i> const& points) { if (SDL_RenderDrawPoints(renderer_,  &points[0], (int)points.size()) != 0) throw Exception{ "SDL_RenderDrawPoints" }; }
	void draw_points(std::vector<Vec2i> const& points, Color const& c) { set_drawcolor(c); draw_points(points); }

	void draw_ray(Vec2i const& orig, Vec2i const& ray) { draw_line(orig, orig + ray); }
	void draw_ray(Vec2i const& orig, Vec2i const& ray, Color const& c) { draw_line(orig, orig + ray, c); }

	void draw_rect(Rect const& rect) { if (SDL_RenderDrawRect(renderer_,  &rect) != 0) throw Exception{ "SDL_RenderDrawRect" }; }
	void draw_rect(Rect const& rect, Color const& c) { set_drawcolor(c); draw_rect(rect); }

	void draw_rects(std::vector<Rect> const& rects) { if (SDL_RenderDrawRects(renderer_, &rects[0], (int)rects.size()) != 0) throw Exception{ "SDL_RenderDrawRects" }; }
	void draw_rects(std::vector<Rect> const& rects, const Color& c) { set_drawcolor(c); draw_rects(rects); }

	void fill_rect(Rect const& rect) { if (SDL_RenderFillRect(renderer_, &rect) != 0) throw Exception{ "SDL_RenderFillRect" }; }
	void fill_rect(Rect const& rect, Color const& c) { set_drawcolor(c); fill_rect(rect); }

	void fill_rects(std::vector<Rect> const& rects) { if (SDL_RenderFillRects(renderer_, &rects[0], (int)rects.size()) != 0) throw Exception{ "SDL_RenderDrawRects" }; }
	void fill_rects(std::vector<Rect> const& rects, Color const& c) { set_drawcolor(c); fill_rects(rects); }

private:
	SDL_Renderer* renderer_;
};

} // namespace sdl