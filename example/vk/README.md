# cpp-sdl2 Vulkan example

This is a small "hello vulkan" program, but that uses cpp-sdl2 for enumerating device extensions and to create the platform specific vkSurface object

the interesting code for our demo amont to this : 

Include like this (with an OpenGL loader of your choice)
```cpp
#define NOMINMAX
#include <fstream>
#include <iostream>
#include <set>
#include <vector>

#include <vulkan/vulkan.hpp>

// Define the flollowing token
#define CPP_SDL2_VK_WINDOW
#include <cpp-sdl2/sdl.hpp>

// use validation layers only on debug builds
#if defined(_DEBUG) || defined(DEBUG)
#define VK_APP_USE_LAYER true
#endif

#ifndef VK_APP_USE_LAYER
#define VK_APP_USE_LAYER false
#endif
```

Note that we also manage here a flag to know if we should activate validation layers or not



//Begining of `main()`
```cpp
	uint32_t width	= 800;
	uint32_t height = 600;

	// Create widnow with the SDL_WINDOW_VULKAN configuration flag
	sdl::Window window("Vulkan", {int(width), int(height)}, SDL_WINDOW_VULKAN);
	auto		vk_instance_exts = window.vk_get_instance_extensions();
	auto		vk_layers =
		std::vector<const char*>{"VK_LAYER_LUNARG_standard_validation"};

	const vk::ApplicationInfo vk_app_info{"Vulkan",
										  VK_MAKE_VERSION(1, 0, 0),
										  "not_an_engine",
										  VK_MAKE_VERSION(1, 0, 0)};

	if (VK_APP_USE_LAYER) vk_instance_exts.push_back("VK_EXT_debug_utils");

	auto vk_instance = vk::createInstanceUnique(vk::InstanceCreateInfo{
		{},
		&vk_app_info,
		VK_APP_USE_LAYER ? uint32_t(vk_layers.size()) : 0,
		VK_APP_USE_LAYER ? vk_layers.data() : nullptr,
		uint32_t(vk_instance_exts.size()),
		vk_instance_exts.data()});

#if VK_APP_USE_LAYER
	auto vk_messenger = vk_instance->createDebugUtilsMessengerEXTUnique(
		vk::DebugUtilsMessengerCreateInfoEXT{
			{},
			vk::DebugUtilsMessageSeverityFlagBitsEXT::eError
				| vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning
				| vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose
				| vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo,
			vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral
				| vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation
				| vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance,
			vk_debug_callback // pointer to our own function here
		},
		nullptr,
		vk::DispatchLoaderDynamic{vk_instance.get()});
#endif

	// Create a surface
	auto surface = window.vk_create_unique_surface(vk_instance.get());
```
