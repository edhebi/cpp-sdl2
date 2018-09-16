#pragma once

#include <SDL2/SDL.h>

#include <string>

#include "exception.hpp"

namespace sdl
{

///Represent a shared object (dynamic library, dynamically loaded library, module, plugin...). Permit to retrieve callable function from them.
///This class automatically manages the lifetime of the loaded library
class SharedObject
{
	using SharedObjectHandle = void*;
	SharedObjectHandle handle_ = nullptr;

public:
	using FunctionAddress = void*;

	SharedObject(std::string const& objectName) :
		handle_{SDL_LoadObject(objectName.c_str())}
	{
		if (!handle_)
			throw Exception("SDL_LoadObject");
	}

	~SharedObject()
	{
		SDL_UnloadObject(handle_);
	}

	// This class isn't copyable
	SharedObject(SharedObject const&) = delete;
	SharedObject& operator=(SharedObject const&) = delete;

	SharedObject(SharedObject&& other)
	{
		handle_ = other.handle_;
		other.handle_ = nullptr;
	}

	SharedObject& operator=(SharedObject&& other)
	{
		handle_		   = other.handle_;
		other.handle_ = nullptr;
	}

	///Retrieve the raw address of a function inside the owned object. User has to cast this to the correct function pointer type
	FunctionAddress function_pointer(std::string const& functionName) const
	{
		auto address = SDL_LoadFunction(handle_, functionName.c_str());
		if (!address)
			throw Exception("SDL_LoadFunction");
		return address;
	}

	///Syntactic sugar overload, provide you a way to specify the actual type of the function pointer
	/// e.g: object.function_pointer<returnType(*)(args)>("name");
	template <typename FuncitonPointerSignature>
	FuncitonPointerSignature funciton_pointer(std::string const& functionName) const
	{
		return reinterpret_cast<FuncitonPointerSignature>(function_pointer(functionName));
	}
};

}
