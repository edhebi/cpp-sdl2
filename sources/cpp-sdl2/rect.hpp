#pragma once

#include <SDL_rect.h>
#include <algorithm>

#include "vec2.hpp"

namespace sdl
{
///sdl::Rect, C++ wrapping of SDL_Rect
class Rect : public SDL_Rect
{
public:
	///Construct a Rect initialized at 0
	constexpr Rect() : SDL_Rect{0, 0, 0, 0} {}

	///Construct a Rect with the given dimensions
	///\param x Position on X axis
	///\param y Position on Y axis
	///\param w Size on X axis
	///\param h Size on Y axis
	constexpr Rect(int x, int y, int w, int h) : SDL_Rect{x, y, w, h} {}

	///Construct a Rect with the given dimensions
	///\param corner X/Y position on screen as a 2D vector
	///\param size X/Y size on screen as a 2D vector
	constexpr Rect(Vec2i const& corner, Vec2i const& size)
		: SDL_Rect{corner.x, corner.y, size.x, size.y}
	{
	}

	///Copy a Rect
	constexpr Rect(SDL_Rect const& r) : SDL_Rect{r} {}

	///Copy a Rect
	Rect(Rect const&) noexcept = default;

	///Move from a Rect
	Rect(Rect&&) noexcept = default;

	///Contruct a Rect with dimensions around a center point
	///\param cx position of the center on the X axis
	///\param cy position of the center on the Y axis
	///\param w Size on X axis
	///\param h Size on Y axis
	static constexpr Rect from_center(int cx, int cy, int w, int h)
	{
		return Rect{cx - w / 2, cy - h / 2, w, h};
	}

	///Construct a Rect with dimensions around a center point
	///\param center X/Y position of the center point as a 2D vector
	static constexpr Rect from_center(Vec2i const& center, Vec2i const& size)
	{
		return Rect{center - size / 2, size};
	}

	///Construct a rect from 2 corner points
	///\param x1 X position of first point
	///\param y1 Y position of first point
	///\param x2 X position of second point
	///\param x2 Y position of second point
	static constexpr Rect from_corners(int x1, int y1, int x2, int y2)
	{
		return Rect(x1, y1, x2 - x1, y2 - y1);
	}

	///Construct a rect from 2 corner points
	///\param corner1 X/Y position of the first corner as a 2D vector
	///\param corner2 X/Y position of the second corner as a 2D vector
	static constexpr Rect from_corners(Vec2i const& corner1, Vec2i const& corner2)
	{
		return Rect(corner1.x, corner1.y, corner2.x - corner1.x, corner2.y - corner1.y);
	}

	///Copy assign a Rect
	Rect& operator=(Rect const&) noexcept = default;
	///Move assign a Rect
	Rect& operator=(Rect&&) noexcept = default;

	///Returns true if the two rect are the same
	bool operator==(Rect const& other) const { return SDL_RectEquals(this, &other); }

	///Return the 'min X' position of the Rect
	constexpr int x1() const { return x; }
	///Return the 'max X' position of the Rect
	constexpr int x2() const { return x + w; }
	///Return the 'min Y' position of the Rect
	constexpr int y1() const { return y; }
	///Return the 'max Y' position of the Rect
	constexpr int y2() const { return y + h; }

	///Get the bottom left corner position
	Vec2i botleft() const { return Vec2i{x1(), y1()}; }
	///Get the bottom right corner position
	Vec2i botright() const { return Vec2i{x2(), y1()}; }
	///Get the top left corner position
	Vec2i topleft() const { return Vec2i{x1(), y2()}; }
	///Get the top right corner position
	Vec2i topright() const { return Vec2i{x2(), y2()}; }

	///Get the size of the Rect
	Vec2i size() const { return Vec2i{w, h}; }
	///Get the center of the Rect
	Vec2i center() const { return Vec2i{x + w / 2, y + h / 2}; }

	///Return true if this Rect is empty
	bool is_empty() const { return SDL_RectEmpty(this); }

	///Return true if this rect contains the given point
	///\param px X position of the point
	///\param py Y position of the point
	bool contains(int px, int py) const
	{
		return px >= x1() && px < x2() && py >= y1() && py < y2();
	}

	///Return true if this rect contains the given point
	///\param point The X/Y coordinates of the point as a vector 2D
	bool contains(Vec2i const& point) const { return contains(point.x, point.y); }

	///Return true if this rect intersect another rect
	bool intersects(Rect const& r) const
	{
		return x1() < r.x2() && x2() > r.x1() && y1() < r.y2() && y2() > r.y1();
	}

	///Return true if this rect intersect the line
	bool intersects(Vec2i const& p1, Vec2i const& p2) const
	{
		/* Even if SDL_IntersectRectAndLine don't modify it's arguments,
		it doesn't use const* of int */
		auto p1mut = const_cast<Vec2i&>(p1);
		auto p2mut = const_cast<Vec2i&>(p2);

		return SDL_IntersectRectAndLine(this, &p1mut.x, &p1mut.y, &p2mut.x, &p2mut.y);
	}

	///Return the intersection of the two rects
	Rect inter(Rect const& r) const
	{
		Rect tmp;
		SDL_IntersectRect(this, &r, &tmp);
		return tmp;
	}

	// TODO : find a way to name this ...
	///Return the union of the two rects
	Rect get_union(Rect const& r) const
	{
		Rect tmp;
		SDL_UnionRect(this, &r, &tmp);
		return tmp;
	}
};

} // namespace sdl
