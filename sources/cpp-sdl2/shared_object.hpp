#pragma once

#include <SDL.h>

#include <string>

#include "exception.hpp"

namespace sdl
{
///\brief Represent a shared object (dynamic library, dynamically loaded library, module, plugin...).
///
///This class wraps a platform-specific "handle" to a loaded library.
///An instanciated SharedObject permit to retrieve callable function from them in a cross-platform way.
///This class automatically manages the lifetime of the loaded library for you
class SharedObject
{
	///Types should be at least named. Alias void* to "SharedObjectHandle*
	using SharedObjectHandle = void*;
	///This class wrap an handle to a shared object
	SharedObjectHandle handle_ = nullptr;

public:
	///Alias void* to a descriptive "pointer to function" typename
	using FunctionAddress = void*;

	///Get a shared object ( = Load the named library dynamically)
	///\param objectName Name of the library.
	SharedObject(std::string const& objectName)
		: handle_{SDL_LoadObject(objectName.c_str())}
	{
		if (!handle_) throw Exception("SDL_LoadObject");
	}

	///Automatically unload the library for you
	~SharedObject() { SDL_UnloadObject(handle_); }

	///This class isn't copyable
	SharedObject(SharedObject const&) = delete;

	///This class isn't copyable
	SharedObject& operator=(SharedObject const&) = delete;

	/// Move ctor
	SharedObject(SharedObject&& other) noexcept
	{
		handle_		  = other.handle_;
		other.handle_ = nullptr;
	}

	///Move shared object into this one
	SharedObject& operator=(SharedObject&& other) noexcept
	{
		if (handle_) SDL_UnloadObject(handle_);
		handle_		  = other.handle_;
		other.handle_ = nullptr;

		return *this;
	}

	///Retrieve the raw address of a function inside the owned object. User has to cast this to the correct function pointer type
	FunctionAddress function_pointer(std::string const& functionName) const
	{
		auto address = SDL_LoadFunction(handle_, functionName.c_str());
		if (!address) throw Exception("SDL_LoadFunction");
		return address;
	}

	///Syntactic sugar overload, provide you a way to specify the actual type of the function pointer
	/// e.g: mySharedObject.function_pointer<returnType (*) (args)>("nameOfExportedFunction");
	///\param functionName The name of a callable symbol that can be found in the loaded library
	template<typename FunctionPointerSignature>
	FunctionPointerSignature function_pointer(
		std::string const& functionName) const
	{
		return reinterpret_cast<FunctionPointerSignature>(
			function_pointer(functionName));
	}
};

} // namespace sdl
