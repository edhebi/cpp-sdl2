#pragma once

#include <algorithm>
#include <SDL2/SDL_rect.h>

#include "vec2.hpp"

namespace sdl
{

class Rect : public SDL_Rect
{
public:
	constexpr Rect() : SDL_Rect{ 0, 0, 0, 0 } {}
	constexpr Rect(int x, int y, int w, int h) : SDL_Rect{ x, y, w, h } {}

	constexpr Rect(Vec2i const& corner, Vec2i const& size) 
		: SDL_Rect{ corner.x, corner.y, size.x, size.y } {}

	constexpr Rect(SDL_Rect const& r) : SDL_Rect{ r } {}

	Rect(Rect const&) noexcept = default;
	Rect(Rect&&) noexcept = default;

	static constexpr Rect from_center(int cx, int cy, int w, int h)
	{
		return Rect{ cx - w / 2, cy - h / 2, w, h };
	}

	static constexpr Rect from_center(Vec2i const& center, Vec2i const& size)
	{
		return Rect{ center - size / 2, size };
	}
	
	static constexpr Rect from_corners(int x1, int y1, int x2, int y2)
	{
		return Rect(x1, y1, x2 - x1, y2 - y1);
	}

	Rect& operator=(Rect const&) noexcept = default;
	Rect& operator=(Rect&&) noexcept = default;

	bool operator==(Rect const& other) const
	{
		return SDL_RectEquals(this, &other);
	}

	constexpr int x1() const { return x; }
	constexpr int x2() const { return x + w; }
	constexpr int y1() const { return y; }
	constexpr int y2() const { return y + h; }

	Vec2i botleft()  const { return Vec2i{ x1(), y1() }; }
	Vec2i botright() const { return Vec2i{ x2(), y1() }; }
	Vec2i topleft()  const { return Vec2i{ x1(), y2() }; }
	Vec2i topright() const { return Vec2i{ x2(), y2() }; }

	Vec2i size() const { return Vec2i{ w, h }; }
	Vec2i center() const { return Vec2i{ x + w / 2, y + h / 2 }; }

	bool is_empty() const
	{
		return SDL_RectEmpty(this);
	}

	bool contains(int px, int py) const
	{
		return px >= x1() && px < x2() && py >= y1() && py < y2();
	}

	bool contains(Vec2i const& point) const
	{
		return contains(point.x, point.y);
	}

	bool intersects(Rect const& r) const
	{
		return x1() < r.x2() && x2() > r.x1() && y1() < r.y2() && y2() > r.y1();
	}

	bool intersect(Vec2i const& p1, Vec2i const& p2) const
	{
		/* Even if SDL_IntersectRectAndLine don't modify it's arguments,
		it doesn't use const* of int */
		auto p1mut = const_cast<Vec2i&>(p1);
		auto p2mut = const_cast<Vec2i&>(p2);

		return SDL_IntersectRectAndLine(this, &p1mut.x, &p1mut.y, &p2mut.x, &p2mut.y);
	}

	Rect inter(Rect const& r) const
	{
		Rect tmp;
		SDL_IntersectRect(this, &r, &tmp);
		return tmp;
	}

	// TODO : find a way to name this ...
	Rect get_union(Rect const& r) const
	{
		Rect tmp;
		SDL_UnionRect(this, &r, &tmp);
		return tmp;
	}

};

} // namespace sdl