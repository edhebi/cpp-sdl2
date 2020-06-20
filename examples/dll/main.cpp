#include <cpp-sdl2/sdl.hpp>
#include <iostream>
#include <memory>

// define pointer types
using dll_function_ptr = void (*)();
using dll_get_int_ptr  = int (*)();

int main(int argc, char* argv[])
{
	// Here's how you load a dynamiy library
	auto so_instance = sdl::SharedObject("cpp_sdl2_example_dll_shared.dll");

	// Load function pointer
	const dll_function_ptr dll_function =
		(dll_function_ptr)so_instance.function_pointer( // note the pointer cast
			"print_hello");

	// You can specify the function signature as the function pointer type in
	// template argument. Avoid ugly casts like the one above
	auto get_the_answer_to_life_the_universe_and_everything =
		so_instance.function_pointer<dll_get_int_ptr>(
			"get_the_answer_to_life_the_universe_and_everything");

	// Call the functions
	dll_function();
	std::cout << "answer is : "
			  << get_the_answer_to_life_the_universe_and_everything() << "\n";

	// The following show how to catch errors gracefully:
	// Exception are thrown if library or procedure cannot be found
	std::unique_ptr<sdl::SharedObject> dll_not_here;
	dll_function_ptr				   print_nuclear_code = nullptr;
	try
	{
		dll_not_here = std::make_unique<sdl::SharedObject>("magical_library");
	}
	catch (const std::exception& e)
	{
		std::cerr << "Cannot load DLL " << e.what() << "\n";
	}

	try
	{
		print_nuclear_code =
			(dll_function_ptr)so_instance.function_pointer("print_secret");
	}
	catch (const std::exception& e)
	{
		std::cerr << "Failed to locate function in library " << e.what()
				  << "\n";
	}

	return 0;
}
