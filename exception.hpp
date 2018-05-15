#pragma once

#include <stdexcept>
#include <string>
#include <string_view>

#include <SDL2/SDL_error.h>

namespace sdl
{

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

} // namespace sdl
