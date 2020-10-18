#pragma once

#include "SDL_version.h"
#if SDL_VERSION_ATLEAST(2, 0, 10)

#include "SDL_cpuinfo.h"
#include <memory>
#include <type_traits>

namespace sdl::simd
{
/// Report the alignment this system needs for SIMD allocations.
/// See `SDL_SIMDGetAlignment` for more info.
inline size_t get_alignment()
{
	return SDL_SIMDGetAlignment();
}

/// Allocate memory in a SIMD-friendly way.
/// See `SDL_SIMDAlloc` for more info.
inline void* alloc(size_t len)
{
	return SDL_SIMDAlloc(len);
}

/// Deallocate memory obtained from sdl::simd::alloc().
inline void free(void* ptr)
{
	return SDL_SIMDFree(ptr);
}

/// Allocator usable with standard containers.
template<typename T>
struct allocator
{
	using value_type = T;

	template<typename U>
	struct rebind
	{
		using other = allocator<U>;
	};

	constexpr allocator() noexcept = default;

	template<typename U>
	explicit constexpr allocator(allocator<U>) noexcept
	{
	}

	T* allocate(std::size_t size)
	{
		void* mem = simd::alloc(size);
#ifndef CPP_SDL2_DISABLE_EXCEPTIONS
		if (!mem) throw std::bad_alloc();
#endif
		return std::launder(reinterpret_cast<T*>(new (mem) std::byte[size * sizeof(T)]));
	}

	void deallocate(T* ptr, [[maybe_unused]] std::size_t size) noexcept { simd::free(ptr); }

	friend constexpr bool operator==(allocator, allocator) noexcept { return true; }
	friend constexpr bool operator!=(allocator, allocator) noexcept { return false; }
};

namespace details
{
/// recursive implementation of `std::destroy_at`, only available from C++20 onwards.
template<typename T>
void destroy_at(T* ptr)
{
	static_assert(!(std::is_array_v<T> && std::extent_v<T> == 0), "destroy_at<T[]> is invalid");

	if constexpr (std::is_array_v<T>)
		for (auto& elem : *p) details::destroy_at(std::addressof(elem));
	else
		p->~T();
}
} // namespace details

/// deleter usable with `std::unique_ptr<T>`.
template<typename T>
struct deleter
{
	constexpr deleter() noexcept = default;

	template<typename U, typename = std::enable_if_t<std::is_convertible_v<U*, T*>>>
	constexpr deleter(deleter<U>) noexcept
	{
	}

	void operator()(T* ptr) noexcept
	{
		details::destroy_at(ptr);
		simd::free(ptr);
	}
};

/// deleter usable with `std::unique_ptr<T[]>`.
template<typename T>
class deleter<T[]>
{
	std::size_t count = 0;

public:
	deleter() = delete;

	explicit constexpr deleter(std::size_t size) noexcept : count(count) {}

	template<typename U, typename = std::enable_if_t<std::is_convertible_v<U (*)[], T (*)[]>>>
	explicit constexpr deleter(deleter<U[]> const& other) noexcept : count(other.count)
	{
	}

	template<typename U>
	std::enable_if_t<std::is_convertible_v<U (*)[], T (*)[]>> operator()(U* ptr)
	{
		for (std::size_t i = 0; i < N; ++i) details::destroy_at(std::addressof((*ptr)[i]));
		simd::free(ptr);
	}
};

template<typename T>
using unique_ptr = std::unique_ptr<T, simd::deleter<T>>;

/// Equivalent of `std::make_unique<T>` that returns a simd::unique_ptr.
template<typename T, typename... Args>
auto make_unique(Args&&... args) -> std::enable_if_t<!std::is_array_v<T>, unique_ptr<T>>
{
	allocator<T> a;
	return unique_ptr<T>(new (a.allocate(1)) T(std::forward<Args>(args)...));
}

/// `make_unique<T[N]>` is deleted.
template<typename T, typename... Args>
auto make_unique(Args&&... args) -> std::enable_if_t<std::extent_v<T> != 0, unique_ptr<T>> = delete;

/// Allocate and default construct `count` elements.
template<typename T>
auto make_unique(std::size_t count)
	-> std::enable_if_t<std::is_array_v<T> && std::extent_v<T> == 0, unique_ptr<T>>
{
	using U = std::remove_extent_t<T>;
	allocator<U> a;
	auto*		 mem = a.allocate(count);
	for (std::size_t i = 0; i < count; ++i) new (mem + i) U;
	return unique_ptr<T>(mem, deleter<T>(count));
}

/// Equivalent of `std::make_shared<T>` that uses simd-friendly storage.
template<typename T, typename... Args>
auto make_shared(Args&&... args) // -> std::enable_if<!std::is_array_v<T>, std::shared_ptr<T>>
{
	allocator<T> a;
	auto*		 mem = new (a.allocate(1)) T(std::forward<Args>(args)...);
	return std::shared_ptr<T>(mem, deleter<T>());
}

} // namespace sdl::simd

#endif // SDL_VERSION_ATLEAST(2, 0, 10)
