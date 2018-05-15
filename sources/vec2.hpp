#pragma once

#include <algorithm>
#include <ostream>
#include <SDL2/SDL_rect.h>

namespace sdl::details
{

template <typename T> struct _Vec2_Base
{
	T x, y;
};

} // namespace sdl::details

namespace sdl
{

template <typename T, class Base = details::_Vec2_Base<T>> 
class Vec2 : public Base
{
public:
	constexpr Vec2() : Base{ 0, 0 } {}
	constexpr Vec2(T x, T y) : Base{ x, y } {}

	constexpr Vec2(SDL_Point const& p) : Base{ p.x, p.y } {}

	Vec2(Vec2 const&) noexcept = default;
	Vec2(Vec2&&) noexcept = default;

	template <typename A>
	static constexpr Vec2 from_polar(A alpha, T radius)
	{
		return Vec2{ radius * std::cos(alpha), radius * std::sin(alpha) };
	}

	Vec2& operator=(Vec2 const&) noexcept = default;
	Vec2& operator=(Vec2&&) noexcept = default;
	
	Vec2 operator-() const { return Vec2{ -x, -y }; }
	
	Vec2& operator+=(Vec2 const& other) { x += other.x; y += other.y; return *this; }
	Vec2& operator-=(Vec2 const& other) { x -= other.x; y -= other.y; return *this; }
	Vec2& operator*=(T value) { x *= value; y *= value; return *this; }
	Vec2& operator/=(T value) { x /= value; y /= value; return *this; }

	Vec2 operator+(Vec2 const& other) const { return Vec2{ x, y } += other; }
	Vec2 operator-(Vec2 const& other) const { return Vec2{ x, y } -= other; }
	Vec2 operator*(T value) const { return Vec2{ x, y } *= value; }
	Vec2 operator/(T value) const { return Vec2{ x, y } /= value; }

	friend Vec2 operator*(T lhs, Vec2 const& rhs) { return rhs * lhs; }

	Vec2 clamped(SDL_Rect const& box) const
	{
		auto r = Vec2{ x, y };
		r.clamp(box);
		return r;
	}

	void clamp(SDL_Rect const& box)
	{
		x = clamp(x, box.x, box.x + box.w);
		y = clamp(y, box.y, box.y + box.h);
	}

	T length() const { return std::sqrt(x * x + y * y); }
	T sqlength() const { return x * x + y * y; }

	bool is_null() const { return x == T(0.0L) || y == T(0.0L); }

	Vec2 normalized() const
	{
		auto r = Vec2{ x, y };
		r.normalize();
		return r;
	}

	void normalize()
	{
		if (is_null()) return;

		auto l = length();
		x /= l;
		y /= l;
	}

	template <typename U>
	explicit operator Vec2<U>() const
	{
		return Vec2<U>{ static_cast<U>(x), static_cast<U>(y) };
	}

	friend std::ostream& operator<<(std::ostream& stream, Vec2 const& v)
	{
		return stream << "(x:" << v.x << ",y:" << v.y << ")";
	}

private:
	T clamp(T x, T a, T b) { return std::min(std::max(x, a), b); }
};


using Vec2i = Vec2<int, SDL_Point>;
using Vec2f = Vec2<float>;
using Vec2d = Vec2<double>;

} // namespace sdl