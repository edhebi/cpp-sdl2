#pragma once

#include <SDL_rect.h>
#include <algorithm>
#include <cmath>
#include <ostream>

///Contains implementation details
namespace sdl::details
{
///base content of a vector 2
template<typename T>
struct Vec2Base
{
	T x, y;
};

} // namespace sdl::details

namespace sdl
{
///Generic templated 2D vector class
template<typename T, class Base = details::Vec2Base<T>>
class Vec2 : public Base
{
public:
	///ctor, that will initialize vector to null vector
	constexpr Vec2() : Base{0, 0} {}
	///ctor that will initialize it to the current value
	constexpr Vec2(T x, T y) : Base{x, y} {}
	///Convert SDL_Point to sdl::Vec2
	constexpr Vec2(SDL_Point const& p) : Base{p.x, p.y} {}

	constexpr Vec2(Vec2 const&) noexcept = default;
	constexpr Vec2(Vec2&&) noexcept		 = default;

	///Convert a vector in polar cordinates to cartesian in the 2D plan
	template<typename A>
	static constexpr Vec2 from_polar(A alpha, T radius)
	{
		return Vec2{radius * std::cos(alpha), radius * std::sin(alpha)};
	}

	Vec2& operator=(Vec2 const&) noexcept = default;
	Vec2& operator=(Vec2&&) noexcept = default;

	///Return the opposite vector
	constexpr Vec2 operator-() const { return Vec2{-Base::x, -Base::y}; }

	///Add vector to this one
	Vec2& operator+=(Vec2 const& other)
	{
		Base::x += other.x;
		Base::y += other.y;
		return *this;
	}
	///Substract vector to this one
	Vec2& operator-=(Vec2 const& other)
	{
		Base::x -= other.x;
		Base::y -= other.y;
		return *this;
	}
	///Multiply vector to this one (x1*x2, y1*y2)
	Vec2& operator*=(T value)
	{
		Base::x *= value;
		Base::y *= value;
		return *this;
	}
	///Divide vector to this one (x1/x2, y1/y2)
	Vec2& operator/=(T value)
	{
		Base::x /= value;
		Base::y /= value;
		return *this;
	}

	///Add vectors together
	constexpr Vec2 operator+(Vec2 const& other) const
	{
		return Vec2{Base::x, Base::y} += other;
	}
	///Substract vectors together
	constexpr Vec2 operator-(Vec2 const& other) const
	{
		return Vec2{Base::x, Base::y} -= other;
	}
	///Multiply vectors together
	constexpr Vec2 operator*(T value) const
	{
		return Vec2{Base::x, Base::y} *= value;
	}
	///Divide vectors together
	constexpr Vec2 operator/(T value) const
	{
		return Vec2{Base::x, Base::y} /= value;
	}

	///Compare vectors, true if they are the same
	constexpr bool operator==(Vec2 const& other) const
	{
		return (Base::x == other.x && Base::y == other.y);
	}
	///Compare vectors, false if they are the same
	constexpr bool operator!=(Vec2 const& other) const
	{
		return !(*this == other);
	}

	///Multiply operator that works on the other side
	friend constexpr Vec2 operator*(T lhs, Vec2 const& rhs)
	{
		return rhs * lhs;
	}

	///Clamp vector inside a box
	///\box rectangle were to clamp vector
	Vec2 clamped(SDL_Rect const& box) const
	{
		auto r = Vec2{Base::x, Base::y};
		r.clamp(box);
		return r;
	}

	///\copydoc clamped(SDL_Rect const& box) const
	void clamp(SDL_Rect const& box)
	{
		Base::x = clamp(Base::x, box.x, box.x + box.w);
		Base::y = clamp(Base::y, box.y, box.y + box.h);
	}

	///Get lenghts of this vector
	T length() const
	{
		return std::sqrt(Base::x * Base::x + Base::y * Base::y);
	}
	///Get squared lenght of this vector
	T sqlength() const { return Base::x * Base::x + Base::y * Base::y; }

	///Return true if this vector is null
	bool is_null() const { return Base::x == T(0.0L) || Base::y == T(0.0L); }

	///Return normalized copy of this vector
	Vec2 normalized() const
	{
		auto r = Vec2{Base::x, Base::y};
		r.normalize();
		return r;
	}

	///Normalize this vector
	void normalize()
	{
		if (is_null()) return;

		const auto l = length();
		Base::x /= l;
		Base::y /= l;
	}

	///Convert this vectort to a vector of <U> type
	template<typename U>
	explicit operator Vec2<U>() const
	{
		return Vec2<U>{static_cast<U>(Base::x), static_cast<U>(Base::y)};
	}

	///Print that vector to stream
	friend std::ostream& operator<<(std::ostream& stream, Vec2 const& v)
	{
		return stream << "(x:" << v.x << ",y:" << v.y << ")";
	}

private:
	///Actual implementation of clamp function
	T clamp(T x, T a, T b) { return std::min(std::max(Base::x, a), b); }
};

///Vector of two integers, that is convertible to/from SDL_Point
using Vec2i = Vec2<int, SDL_Point>;
///Vector of two floats
using Vec2f = Vec2<float>;
///Vector of two double
using Vec2d = Vec2<double>;

} // namespace sdl
