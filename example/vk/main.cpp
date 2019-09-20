#define NOMINMAX
#include <iostream>
#include <set>
#include <vector>
// Define the flollowing token
#define CPP_SDL2_VK_WINDOW
#include <cpp-sdl2/sdl.hpp>
#include <vulkan/vulkan.hpp>

// use validation layers only on debug builds
#if defined(_DEBUG) || defined(DEBUG)
#define VK_APP_USE_LAYER true
#endif

#ifndef VK_APP_USE_LAYER
#define VK_APP_USE_LAYER false
#endif

static VKAPI_ATTR VkBool32 VKAPI_CALL vk_debug_callback(
	VkDebugUtilsMessageSeverityFlagBitsEXT		severity,
	VkDebugUtilsMessageTypeFlagsEXT				message_type,
	const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
	void*										user_data)
{
	(void)message_type, (void)severity, (void)user_data;
	std::cerr << "validation layer: " << callback_data->pMessage << std::endl;
	return VK_FALSE;
}

int main(int argc, char* argv[])
{
	(void)argc;
	(void)argv;

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
	auto vk_physical_devices = vk_instance->enumeratePhysicalDevices();
	auto vk_physical_device	 = vk_physical_devices.back();
	auto queue_family_properties =
		vk_physical_device.getQueueFamilyProperties();

	size_t graphicsQueueFamilyIndex = std::distance(
		queue_family_properties.begin(),
		std::find_if(
			queue_family_properties.begin(),
			queue_family_properties.end(),
			[](vk::QueueFamilyProperties const& qfp) {
				return qfp.queueFlags & vk::QueueFlagBits::eGraphics;
			}));

	size_t presentQueueFamilyIndex = 0u;
	for (size_t i = 0; i < queue_family_properties.size(); i++)
	{
		if (vk_physical_device.getSurfaceSupportKHR(
				static_cast<uint32_t>(i), surface.get()))
		{
			presentQueueFamilyIndex = i;
		}
	}
	std::set<size_t> uniqueQueueFamilyIndices = {graphicsQueueFamilyIndex,
												 presentQueueFamilyIndex};

	std::vector<uint32_t> FamilyIndices = {uniqueQueueFamilyIndices.begin(),
										   uniqueQueueFamilyIndices.end()};

	std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;

	float queuePriority = 0.0f;
	for (int queueFamilyIndex : uniqueQueueFamilyIndices)
	{
		queueCreateInfos.push_back(
			vk::DeviceQueueCreateInfo{vk::DeviceQueueCreateFlags(),
									  static_cast<uint32_t>(queueFamilyIndex),
									  1,
									  &queuePriority});
	}

	const std::vector<const char*> deviceExtensions = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME};

	vk::UniqueDevice device =
		vk_physical_device.createDeviceUnique(vk::DeviceCreateInfo(
			vk::DeviceCreateFlags(),
			queueCreateInfos.size(),
			queueCreateInfos.data(),
			0,
			nullptr,
			deviceExtensions.size(),
			deviceExtensions.data()));

	uint32_t imageCount = 2;

	struct SM
	{
		vk::SharingMode sharingMode;
		uint32_t		familyIndicesCount;
		uint32_t*		familyIndicesDataPtr;
	} sharingModeUtil{
		(graphicsQueueFamilyIndex != presentQueueFamilyIndex)
			? SM{vk::SharingMode::eConcurrent, 2u, FamilyIndices.data()}
			: SM{vk::SharingMode::eExclusive,
				 0u,
				 static_cast<uint32_t*>(nullptr)}};

	// needed for validation warnings
	auto capabilities = vk_physical_device.getSurfaceCapabilitiesKHR(*surface);
	auto formats	  = vk_physical_device.getSurfaceFormatsKHR(*surface);

	auto format = vk::Format::eB8G8R8A8Unorm;
	auto extent = vk::Extent2D{width, height};

	vk::SwapchainCreateInfoKHR swapChainCreateInfo(
		{},
		surface.get(),
		imageCount,
		format,
		vk::ColorSpaceKHR::eSrgbNonlinear,
		extent,
		1,
		vk::ImageUsageFlagBits::eColorAttachment,
		sharingModeUtil.sharingMode,
		sharingModeUtil.familyIndicesCount,
		sharingModeUtil.familyIndicesDataPtr,
		vk::SurfaceTransformFlagBitsKHR::eIdentity,
		vk::CompositeAlphaFlagBitsKHR::eOpaque,
		vk::PresentModeKHR::eFifo,
		true,
		nullptr);

	auto swapChain = device->createSwapchainKHRUnique(swapChainCreateInfo);

	std::vector<vk::Image> swapChainImages =
		device->getSwapchainImagesKHR(swapChain.get());

	std::vector<vk::UniqueImageView> imageViews;
	imageViews.reserve(swapChainImages.size());
	for (auto image : swapChainImages)
	{
		vk::ImageViewCreateInfo imageViewCreateInfo(
			vk::ImageViewCreateFlags(),
			image,
			vk::ImageViewType::e2D,
			format,
			vk::ComponentMapping{vk::ComponentSwizzle::eR,
								 vk::ComponentSwizzle::eG,
								 vk::ComponentSwizzle::eB,
								 vk::ComponentSwizzle::eA},
			vk::ImageSubresourceRange{
				vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1});
		imageViews.push_back(
			device->createImageViewUnique(imageViewCreateInfo));
	}

	bool running = true;
	while (running)
	{
		sdl::Event e;
		while (e.poll())
		{
			if (e.type == SDL_QUIT) running = false;
		}
	}

	return 0;
}
