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

static std::array<float, 4> clear_color{.1f, .2f, .3f, 1.f};

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

std::vector<char> read_file(const std::string& filename)
{
	std::ifstream file(filename, std::ios::ate | std::ios::binary);
	if (!file.is_open())
		throw std::runtime_error("failed to open file " + filename);

	size_t			  filesize = (size_t)file.tellg();
	std::vector<char> buffer(filesize);

	file.seekg(0);
	file.read(buffer.data(), filesize);

	return buffer;
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

	// enumerate devices
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

	const auto fragment_data = read_file("../simple_tri.frag.spv");
	const auto vertex_data	 = read_file("../simple_tri.vert.spv");

	vk::ShaderModuleCreateInfo fragment_shader_module_create_info(
		{}, fragment_data.size(), (uint32_t*)fragment_data.data()),
		vertex_shader_module_create_info(
			{}, vertex_data.size(), (uint32_t*)vertex_data.data());

	auto frag_shader_module =
		device->createShaderModuleUnique(fragment_shader_module_create_info);
	auto vert_shader_module =
		device->createShaderModuleUnique(vertex_shader_module_create_info);

	auto vertShaderStageInfo = vk::PipelineShaderStageCreateInfo{
		{}, vk::ShaderStageFlagBits::eVertex, *vert_shader_module, "main"};

	auto fragShaderStageInfo = vk::PipelineShaderStageCreateInfo{
		{}, vk::ShaderStageFlagBits::eFragment, *frag_shader_module, "main"};

	auto pipelineShaderStages = std::vector<vk::PipelineShaderStageCreateInfo>{
		vertShaderStageInfo, fragShaderStageInfo};

	auto vertexInputInfo =
		vk::PipelineVertexInputStateCreateInfo{{}, 0u, nullptr, 0u, nullptr};

	auto inputAssembly = vk::PipelineInputAssemblyStateCreateInfo{
		{}, vk::PrimitiveTopology::eTriangleList, false};

	auto viewport = vk::Viewport{0.0f,
								 0.0f,
								 static_cast<float>(width),
								 static_cast<float>(height),
								 0.0f,
								 1.0f};

	auto scissor = vk::Rect2D{{0, 0}, extent};

	auto viewportState =
		vk::PipelineViewportStateCreateInfo{{}, 1, &viewport, 1, &scissor};

	auto rasterizer = vk::PipelineRasterizationStateCreateInfo{
		{},
		/*depthClamp*/ false,
		/*rasterizeDiscard*/ false,
		vk::PolygonMode::eFill,
		{},
		/*frontFace*/ vk::FrontFace::eCounterClockwise,
		{},
		{},
		{},
		{},
		1.0f};

	auto multisampling = vk::PipelineMultisampleStateCreateInfo{
		{}, vk::SampleCountFlagBits::e1, false, 1.0};

	auto colorBlendAttachment = vk::PipelineColorBlendAttachmentState{
		{},
		/*srcCol*/ vk::BlendFactor::eOne,
		/*dstCol*/ vk::BlendFactor::eZero,
		/*colBlend*/ vk::BlendOp::eAdd,
		/*srcAlpha*/ vk::BlendFactor::eOne,
		/*dstAlpha*/ vk::BlendFactor::eZero,
		/*alphaBlend*/ vk::BlendOp::eAdd,
		vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG
			| vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA};

	auto colorBlending = vk::PipelineColorBlendStateCreateInfo{
		{},
		/*logicOpEnable=*/false,
		vk::LogicOp::eCopy,
		/*attachmentCount=*/1,
		/*colourAttachments=*/&colorBlendAttachment};

	auto pipelineLayout = device->createPipelineLayoutUnique({}, nullptr);

	auto colorAttachment =
		vk::AttachmentDescription{{},
								  format,
								  vk::SampleCountFlagBits::e1,
								  vk::AttachmentLoadOp::eClear,
								  vk::AttachmentStoreOp::eStore,
								  {},
								  {},
								  {},
								  vk::ImageLayout::ePresentSrcKHR};

	auto colourAttachmentRef =
		vk::AttachmentReference{0, vk::ImageLayout::eColorAttachmentOptimal};

	auto subpass = vk::SubpassDescription{{},
										  vk::PipelineBindPoint::eGraphics,
										  /*inAttachmentCount*/ 0,
										  nullptr,
										  1,
										  &colourAttachmentRef};

	auto semaphoreCreateInfo = vk::SemaphoreCreateInfo{};
	auto imageAvailableSemaphore =
		device->createSemaphoreUnique(semaphoreCreateInfo);
	auto renderFinishedSemaphore =
		device->createSemaphoreUnique(semaphoreCreateInfo);

	auto subpassDependency =
		vk::SubpassDependency{VK_SUBPASS_EXTERNAL,
							  0,
							  vk::PipelineStageFlagBits::eColorAttachmentOutput,
							  vk::PipelineStageFlagBits::eColorAttachmentOutput,
							  {},
							  vk::AccessFlagBits::eColorAttachmentRead
								  | vk::AccessFlagBits::eColorAttachmentWrite};

	auto renderPass = device->createRenderPassUnique(vk::RenderPassCreateInfo{
		{}, 1, &colorAttachment, 1, &subpass, 1, &subpassDependency});

	auto pipelineCreateInfo =
		vk::GraphicsPipelineCreateInfo{{},
									   2,
									   pipelineShaderStages.data(),
									   &vertexInputInfo,
									   &inputAssembly,
									   nullptr,
									   &viewportState,
									   &rasterizer,
									   &multisampling,
									   nullptr,
									   &colorBlending,
									   nullptr,
									   *pipelineLayout,
									   *renderPass,
									   0};

	auto pipeline =
		device->createGraphicsPipelineUnique({}, pipelineCreateInfo);

	auto framebuffers = std::vector<vk::UniqueFramebuffer>(imageCount);
	for (size_t i = 0; i < imageViews.size(); i++)
	{
		framebuffers[i] = device->createFramebufferUnique(
			vk::FramebufferCreateInfo{{},
									  *renderPass,
									  1,
									  &(*imageViews[i]),
									  extent.width,
									  extent.height,
									  1});
	}
	auto commandPoolUnique = device->createCommandPoolUnique(
		{{}, static_cast<uint32_t>(graphicsQueueFamilyIndex)});

	std::vector<vk::UniqueCommandBuffer> commandBuffers =
		device->allocateCommandBuffersUnique(vk::CommandBufferAllocateInfo(
			commandPoolUnique.get(),
			vk::CommandBufferLevel::ePrimary,
			framebuffers.size()));

	auto deviceQueue  = device->getQueue(graphicsQueueFamilyIndex, 0);
	auto presentQueue = device->getQueue(presentQueueFamilyIndex, 0);

	for (size_t i = 0; i < commandBuffers.size(); i++)
	{
		auto beginInfo = vk::CommandBufferBeginInfo{};
		commandBuffers[i]->begin(beginInfo);

		vk::ClearValue clearValues{clear_color};

		auto renderPassBeginInfo =
			vk::RenderPassBeginInfo{renderPass.get(),
									framebuffers[i].get(),
									vk::Rect2D{{0, 0}, extent},
									1,
									&clearValues};

		commandBuffers[i]->beginRenderPass(
			renderPassBeginInfo, vk::SubpassContents::eInline);
		commandBuffers[i]->bindPipeline(
			vk::PipelineBindPoint::eGraphics, pipeline.get());
		commandBuffers[i]->draw(3, 1, 0, 0);
		commandBuffers[i]->endRenderPass();
		commandBuffers[i]->end();
	}

	bool running = true;
	while (running)
	{
		sdl::Event e;
		while (e.poll())
		{
			if (e.type == SDL_QUIT) running = false;
		}

		auto imageIndex = device->acquireNextImageKHR(
			swapChain.get(),
			std::numeric_limits<uint64_t>::max(),
			imageAvailableSemaphore.get(),
			{});

		vk::PipelineStageFlags waitStageMask =
			vk::PipelineStageFlagBits::eColorAttachmentOutput;

		auto submitInfo =
			vk::SubmitInfo{1,
						   &imageAvailableSemaphore.get(),
						   &waitStageMask,
						   1,
						   &commandBuffers[imageIndex.value].get(),
						   1,
						   &renderFinishedSemaphore.get()};

		deviceQueue.submit(submitInfo, {});

		auto presentInfo = vk::PresentInfoKHR{1,
											  &renderFinishedSemaphore.get(),
											  1,
											  &swapChain.get(),
											  &imageIndex.value};

		presentQueue.presentKHR(presentInfo);
		device->waitIdle();
	}

	return 0;
}
