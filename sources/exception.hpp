#pragma once


#include <stdexcept>
#include <string>
#include <string_view>

#include <SDL2/SDL_error.h>

#ifdef CPP_SDL2_NOEXCEPTIONS
#include <iostream> //Instead of throwing, we are going to dump the function and error into stderr via std::cerr
#endif

namespace sdl
{

#ifndef CPP_SDL2_NOEXCEPTIONS

class Exception : public std::runtime_error
{
public:
	Exception(std::string const& function)
		: function_{ function }
		, error_{ SDL_GetError() }
		, std::runtime_error{ make_what(function_, error_) }
	{
		SDL_ClearError();
	}

	std::string function() { return function_; };
	std::string error() { return error_; };


private:
	std::string make_what(std::string const& function, std::string const& error)
	{
		return function + " failed: " + error;
	}

	std::string function_;
	std::string error_;
};
#else

#define throw
class Exception
{
public:
	Exception(std::string const& function)
	{
		std::cerr << "Error in : " << function << "\n";
		const auto error = SDL_GetError();
		std::cerr << "Error : " << error << "\n";
		abort();
	}
};


#endif

} // namespace sdl

