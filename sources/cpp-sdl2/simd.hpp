#pragma once

// TODO doxygen this!

#include "SDL_version.h"
#if SDL_VERSION_ATLEAST(2, 0, 10)

#include "SDL_cpuinfo.h"
#include <memory>

namespace sdl::simd
{
size_t get_alignement()
{
	return SDL_SIMDGetAlignment();
}

///Perform allocation of a raw pointer. See SDL_SIMDAlloc for more info about SIMD allocation
void* alloc(size_t len)
{
	return SDL_SIMDAlloc(len);
}

void free(void* ptr)
{
	return SDL_SIMDFree(ptr);
}

///Deleter functor that can be used with std::unique_ptr
template<typename T>
struct deleter
{
	void operator()(T* ptr) { simd::free(static_cast<void*>(ptr)); }
};

// If you want a pointer to be managed in a unique typed way, here you go!
template<typename T>
class unique_ptr
{
public:
	unique_ptr() = default;

	explicit unique_ptr(T* ptr)
	{
		smart_ptr = std::unique_ptr<T, deleter<T>>(ptr);
	}

	// No copy
	unique_ptr(unique_ptr const&) = delete;
	unique_ptr<T>& operator=(unique_ptr const&) = delete;

	// Define move
	unique_ptr<T>& operator=(unique_ptr&& rhs)
	{
		smart_ptr = std::move(rhs.smart_ptr);
		return *this;
	}

	unique_ptr(unique_ptr&& rhs) { *this = std::move(rhs); }

	T* get() { return smart_ptr.get(); }

private:
	std::unique_ptr<T, deleter<T>> smart_ptr = nullptr;
};

// TODO arrays
template<typename T>
unique_ptr<T>&& make_unique()
{
	T* allocated_block = reinterpret_cast<T*>(simd::alloc(sizeof(T)));
	return std::move(unique_ptr<T>(allocated_block));
}

template<typename T>
class array
{
public:
	explicit array(size_t len) : len_(len)
	{
		assert(len >= 0);
		data_ = reinterpret_cast<T*>(simd::alloc(sizeof(T) * len));
	}

	~array() { SDL_SIMDFree((void*)data_); }

	T& operator[](size_t i) { return data_[i]; }

	array& operator=(array<T> const&) = delete;
	array(array<T> const&)			  = delete;

	array& operator=(array<T>&& rhs)

	{
		if (data_) simd::free(data_);
		data_	  = rhs.data_;
		rhs.data_ = nullptr;

		// We know what we are doing here. don't do this at home kids!
		*const_cast<size_t*>(&len_) = rhs.len_;

		return *this;
	}

	array(array<T>&& rhs)
		: len_(0) // len is assigned in the move operator
				  // below
	{
		*this = std::move(rhs);
	}

	size_t size() const { return len_; }

	T* data() { return data_; }

private:
	T*			 data_ = nullptr;
	const size_t len_;
};

template<typename T>
class shared_ptr
{
public:
	shared_ptr() = default;

	explicit shared_ptr(T* ptr)
	{
		smart_ptr = std::shared_ptr<T>(ptr, [](T* ptr) {
			if (ptr) SDL_SIMDFree(static_cast<void*>(ptr));
		});
	}

	shared_ptr<T>& operator=(shared_ptr<T> const& rhs)
	{
		smart_ptr = rhs.smart_ptr;
		return *this;
	}

	shared_ptr(shared_ptr<T> const& rhs) { *this = rhs; }

	shared_ptr<T>& operator=(shared_ptr<T>&& rhs)
	{
		smart_ptr = std::move(rhs.smart_ptr);
		return *this;
	}

	shared_ptr(shared_ptr&& rhs) { *this = std::move(rhs); }

	T* get() { return smart_ptr.get(); }

private:
	std::shared_ptr<T> smart_ptr = nullptr;
};
} // namespace sdl::simd
#endif
