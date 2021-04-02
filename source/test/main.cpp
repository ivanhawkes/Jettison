#include <vulkan/Renderer.h>
#include <vulkan/Model.h>
#include <vulkan/Pipeline.h>
#include <vulkan/Swapchain.h>
#include <vulkan/Window.h>

// GLFW / Vulkan.
#define GLFW_INCLUDE_VULKAN
#include <../glfw/include/GLFW/glfw3.h>

#include <iostream>
#include <stdexcept>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_vulkan.h>


static VkInstance               g_Instance = VK_NULL_HANDLE;
static VkPhysicalDevice         g_PhysicalDevice = VK_NULL_HANDLE;
static VkDevice                 g_Device = VK_NULL_HANDLE;
static uint32_t                 g_QueueFamily = (uint32_t)-1;
static VkQueue                  g_Queue = VK_NULL_HANDLE;
static VkDebugReportCallbackEXT g_DebugReport = VK_NULL_HANDLE;
static VkPipelineCache          g_PipelineCache = VK_NULL_HANDLE;
static VkDescriptorPool         g_DescriptorPool = VK_NULL_HANDLE;

static ImGui_ImplVulkanH_Window g_MainWindowData {};
static int                      g_MinImageCount {2};
static bool                     g_SwapChainRebuild {false};


static void CheckVkResult(VkResult err)
{
	if (err == VK_SUCCESS)
		return;

	std::cerr << "Vulkan error: vkResult = " << err << '\n';
	if (err < 0)
		abort();
}


void ImGuiCreateRenderPass(std::shared_ptr<Jettison::Renderer::DeviceContext> pDeviceContext, std::shared_ptr<Jettison::Renderer::Swapchain> pSwapchain, VkRenderPass& imGuiRenderPass)
{
	VkAttachmentDescription colorAttachment {};
	colorAttachment.format = pSwapchain->GetImageFormat();
	colorAttachment.samples = pDeviceContext->GetMsaaSamples();
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentReference colorAttachmentRef {};
	colorAttachmentRef.attachment = 0;
	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass {};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorAttachmentRef;
	//subpass.pDepthStencilAttachment = nullptr;
	//subpass.pResolveAttachments = nullptr;

	VkSubpassDependency dependency {};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.srcAccessMask = 0;
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

	VkRenderPassCreateInfo renderPassInfo {};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = 1;
	renderPassInfo.pAttachments = &colorAttachment;
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpass;
	renderPassInfo.dependencyCount = 1;
	renderPassInfo.pDependencies = &dependency;

	if (vkCreateRenderPass(pDeviceContext->GetLogicalDevice(), &renderPassInfo, nullptr, &imGuiRenderPass) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create render pass");
	}
}


//void ImGuiCreateDescriptorPool(std::shared_ptr<Jettison::Renderer::DeviceContext> pDeviceContext, VkDescriptorPool& imGuiDescriptorPool)
//{
//	VkResult err;
//
//	VkDescriptorPoolSize pool_sizes[] =
//	{
//		{VK_DESCRIPTOR_TYPE_SAMPLER, 1000},
//		{VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000},
//		{VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000},
//		{VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000},
//		{VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000},
//		{VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000},
//		{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000},
//		{VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000},
//		{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000},
//		{VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000},
//		{VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000}
//	};
//	VkDescriptorPoolCreateInfo pool_info = {};
//	pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
//	pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
//	pool_info.maxSets = 1000 * IM_ARRAYSIZE(pool_sizes);
//	pool_info.poolSizeCount = (uint32_t)IM_ARRAYSIZE(pool_sizes);
//	pool_info.pPoolSizes = pool_sizes;
//
//	err = vkCreateDescriptorPool(pDeviceContext->GetLogicalDevice(), &pool_info, nullptr, &imGuiDescriptorPool);
//	CheckVkResult(err);
//}


void ImGuiInitFontTexture(std::shared_ptr<Jettison::Renderer::DeviceContext> pDeviceContext)
{
	ImGuiIO& io = ImGui::GetIO();

	// Load several different sizes of the VeraMono font supplied by CryTek.
	// NOTE: The first font loaded seems to be the default one used for the UI.
	const std::string droidSansPath {"assets\\fonts\\DroidSans.ttf"};
	io.Fonts->AddFontFromFileTTF(droidSansPath.c_str(), 20.0f);
	io.Fonts->AddFontFromFileTTF(droidSansPath.c_str(), 23.0f);
	io.Fonts->AddFontFromFileTTF(droidSansPath.c_str(), 26.0f);
	io.Fonts->AddFontFromFileTTF(droidSansPath.c_str(), 29.0f);

	const std::string hackRegularPath {"assets\\fonts\\Hack-Regular.ttf"};
	io.Fonts->AddFontFromFileTTF(hackRegularPath.c_str(), 20.0f);
	io.Fonts->AddFontFromFileTTF(hackRegularPath.c_str(), 22.0f);
	io.Fonts->AddFontFromFileTTF(hackRegularPath.c_str(), 23.0f);
	io.Fonts->AddFontFromFileTTF(hackRegularPath.c_str(), 26.0f);
	io.Fonts->AddFontFromFileTTF(hackRegularPath.c_str(), 29.0f);

	const std::string veraMonoPath {"assets\\fonts\\VeraMono.ttf"};
	io.Fonts->AddFontFromFileTTF(veraMonoPath.c_str(), 20.0f);
	io.Fonts->AddFontFromFileTTF(veraMonoPath.c_str(), 22.0f);
	io.Fonts->AddFontFromFileTTF(veraMonoPath.c_str(), 23.0f);
	io.Fonts->AddFontFromFileTTF(veraMonoPath.c_str(), 26.0f);
	io.Fonts->AddFontFromFileTTF(veraMonoPath.c_str(), 29.0f);

	io.Fonts->AddFontDefault();

	//ImGui_ImplVulkan_CreateFontsTexture(commandBuffer);
	VkCommandBuffer command_buffer = pDeviceContext->BeginSingleTimeCommands();
	ImGui_ImplVulkan_CreateFontsTexture(command_buffer);
	pDeviceContext->EndSingleTimeCommands(command_buffer);
}


static void FrameRender(std::shared_ptr<Jettison::Renderer::DeviceContext> pDeviceContext, ImGui_ImplVulkanH_Window* wd, ImDrawData* draw_data)
{
	VkResult err;

	VkSemaphore image_acquired_semaphore = wd->FrameSemaphores[wd->SemaphoreIndex].ImageAcquiredSemaphore;
	VkSemaphore render_complete_semaphore = wd->FrameSemaphores[wd->SemaphoreIndex].RenderCompleteSemaphore;
	err = vkAcquireNextImageKHR(pDeviceContext->GetLogicalDevice(), wd->Swapchain, UINT64_MAX, image_acquired_semaphore, VK_NULL_HANDLE, &wd->FrameIndex);
	if (err == VK_ERROR_OUT_OF_DATE_KHR)
	{
		g_SwapChainRebuild = true;
		return;
	}
	CheckVkResult(err);

	ImGui_ImplVulkanH_Frame* fd = &wd->Frames[wd->FrameIndex];
	{
		err = vkWaitForFences(pDeviceContext->GetLogicalDevice(), 1, &fd->Fence, VK_TRUE, UINT64_MAX);    // wait indefinitely instead of periodically checking
		CheckVkResult(err);

		err = vkResetFences(pDeviceContext->GetLogicalDevice(), 1, &fd->Fence);
		CheckVkResult(err);
	}
	{
		err = vkResetCommandPool(pDeviceContext->GetLogicalDevice(), fd->CommandPool, 0);
		CheckVkResult(err);
		VkCommandBufferBeginInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		err = vkBeginCommandBuffer(fd->CommandBuffer, &info);
		CheckVkResult(err);
	}
	{
		VkRenderPassBeginInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		info.renderPass = wd->RenderPass;
		info.framebuffer = fd->Framebuffer;
		info.renderArea.extent.width = wd->Width;
		info.renderArea.extent.height = wd->Height;
		info.clearValueCount = 1;
		info.pClearValues = &wd->ClearValue;
		vkCmdBeginRenderPass(fd->CommandBuffer, &info, VK_SUBPASS_CONTENTS_INLINE);
	}

	// Record dear imgui primitives into command buffer
	ImGui_ImplVulkan_RenderDrawData(draw_data, fd->CommandBuffer);

	// Submit command buffer
	vkCmdEndRenderPass(fd->CommandBuffer);
	{
		VkPipelineStageFlags wait_stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		VkSubmitInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		info.waitSemaphoreCount = 1;
		info.pWaitSemaphores = &image_acquired_semaphore;
		info.pWaitDstStageMask = &wait_stage;
		info.commandBufferCount = 1;
		info.pCommandBuffers = &fd->CommandBuffer;
		info.signalSemaphoreCount = 1;
		info.pSignalSemaphores = &render_complete_semaphore;

		err = vkEndCommandBuffer(fd->CommandBuffer);
		CheckVkResult(err);
		err = vkQueueSubmit(pDeviceContext->GetGraphicsQueue(), 1, &info, fd->Fence);
		CheckVkResult(err);
	}
}


static void FramePresent(std::shared_ptr<Jettison::Renderer::DeviceContext> pDeviceContext, ImGui_ImplVulkanH_Window* wd)
{
	if (g_SwapChainRebuild)
		return;

	VkSemaphore render_complete_semaphore = wd->FrameSemaphores[wd->SemaphoreIndex].RenderCompleteSemaphore;
	VkPresentInfoKHR info = {};
	info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	info.waitSemaphoreCount = 1;
	info.pWaitSemaphores = &render_complete_semaphore;
	info.swapchainCount = 1;
	info.pSwapchains = &wd->Swapchain;
	info.pImageIndices = &wd->FrameIndex;
	VkResult err = vkQueuePresentKHR(pDeviceContext->GetGraphicsQueue(), &info);
	if (err == VK_ERROR_OUT_OF_DATE_KHR)
	{
		g_SwapChainRebuild = true;
		return;
	}
	CheckVkResult(err);
	wd->SemaphoreIndex = (wd->SemaphoreIndex + 1) % wd->ImageCount; // Now we can use the next set of semaphores
}


static void SetupVulkan(const char** extensions, uint32_t extensions_count)
{
	VkResult err;

	// Create Vulkan Instance
	{
		VkInstanceCreateInfo create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		create_info.enabledExtensionCount = extensions_count;
		create_info.ppEnabledExtensionNames = extensions;

#ifdef IMGUI_VULKAN_DEBUG_REPORT
		// Enabling multiple validation layers grouped as LunarG standard validation
		const char* layers[] = {"VK_LAYER_LUNARG_standard_validation"};
		create_info.enabledLayerCount = 1;
		create_info.ppEnabledLayerNames = layers;

		// Enable debug report extension (we need additional storage, so we duplicate the user array to add our new extension to it)
		const char** extensions_ext = (const char**)malloc(sizeof(const char*) * (extensions_count + 1));
		memcpy(extensions_ext, extensions, extensions_count * sizeof(const char*));
		extensions_ext[extensions_count] = "VK_EXT_debug_report";
		create_info.enabledExtensionCount = extensions_count + 1;
		create_info.ppEnabledExtensionNames = extensions_ext;

		// Create Vulkan Instance
		err = vkCreateInstance(&create_info, nullptr, &g_Instance);
		CheckVkResult(err);
		free(extensions_ext);

		// Get the function pointer (required for any extensions)
		auto vkCreateDebugReportCallbackEXT = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(g_Instance, "vkCreateDebugReportCallbackEXT");
		IM_ASSERT(vkCreateDebugReportCallbackEXT != NULL);

		// Setup the debug report callback
		VkDebugReportCallbackCreateInfoEXT debug_report_ci = {};
		debug_report_ci.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
		debug_report_ci.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT | VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT;
		debug_report_ci.pfnCallback = debug_report;
		debug_report_ci.pUserData = NULL;
		err = vkCreateDebugReportCallbackEXT(g_Instance, &debug_report_ci, nullptr, &g_DebugReport);
		CheckVkResult(err);
#else
		// Create Vulkan Instance without any debug feature
		err = vkCreateInstance(&create_info, nullptr, &g_Instance);
		CheckVkResult(err);
		IM_UNUSED(g_DebugReport);
#endif
	}

	// Select GPU
	{
		uint32_t gpu_count;
		err = vkEnumeratePhysicalDevices(g_Instance, &gpu_count, NULL);
		CheckVkResult(err);
		IM_ASSERT(gpu_count > 0);

		VkPhysicalDevice* gpus = (VkPhysicalDevice*)malloc(sizeof(VkPhysicalDevice) * gpu_count);
		err = vkEnumeratePhysicalDevices(g_Instance, &gpu_count, gpus);
		CheckVkResult(err);

		// If a number >1 of GPUs got reported, you should find the best fit GPU for your purpose
		// e.g. VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU if available, or with the greatest memory available, etc.
		// for sake of simplicity we'll just take the first one, assuming it has a graphics queue family.
		g_PhysicalDevice = gpus[0];
		free(gpus);
	}

	// Select graphics queue family
	{
		uint32_t count;
		vkGetPhysicalDeviceQueueFamilyProperties(g_PhysicalDevice, &count, NULL);
		VkQueueFamilyProperties* queues = (VkQueueFamilyProperties*)malloc(sizeof(VkQueueFamilyProperties) * count);
		vkGetPhysicalDeviceQueueFamilyProperties(g_PhysicalDevice, &count, queues);
		for (uint32_t i = 0; i < count; i++)
			if (queues[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
			{
				g_QueueFamily = i;
				break;
			}
		free(queues);
		IM_ASSERT(g_QueueFamily != (uint32_t)-1);
	}

	// Create Logical Device (with 1 queue)
	{
		int device_extension_count = 1;
		const char* device_extensions[] = {"VK_KHR_swapchain"};
		const float queue_priority[] = {1.0f};
		VkDeviceQueueCreateInfo queue_info[1] = {};
		queue_info[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queue_info[0].queueFamilyIndex = g_QueueFamily;
		queue_info[0].queueCount = 1;
		queue_info[0].pQueuePriorities = queue_priority;
		VkDeviceCreateInfo create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		create_info.queueCreateInfoCount = sizeof(queue_info) / sizeof(queue_info[0]);
		create_info.pQueueCreateInfos = queue_info;
		create_info.enabledExtensionCount = device_extension_count;
		create_info.ppEnabledExtensionNames = device_extensions;
		err = vkCreateDevice(g_PhysicalDevice, &create_info, nullptr, &g_Device);
		CheckVkResult(err);
		vkGetDeviceQueue(g_Device, g_QueueFamily, 0, &g_Queue);
	}

	// Create Descriptor Pool
	{
		VkDescriptorPoolSize pool_sizes[] =
		{
			{VK_DESCRIPTOR_TYPE_SAMPLER, 1000},
			{VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000},
			{VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000},
			{VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000},
			{VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000},
			{VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000},
			{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000},
			{VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000},
			{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000},
			{VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000},
			{VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000}
		};
		VkDescriptorPoolCreateInfo pool_info = {};
		pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
		pool_info.maxSets = 1000 * IM_ARRAYSIZE(pool_sizes);
		pool_info.poolSizeCount = (uint32_t)IM_ARRAYSIZE(pool_sizes);
		pool_info.pPoolSizes = pool_sizes;
		err = vkCreateDescriptorPool(g_Device, &pool_info, nullptr, &g_DescriptorPool);
		CheckVkResult(err);
	}
}


static void SetupVulkanWindow(/*std::shared_ptr<Jettison::Renderer::DeviceContext> pDeviceContext, std::shared_ptr<Jettison::Renderer::Swapchain> pSwapchain,*/
	ImGui_ImplVulkanH_Window* wd, VkSurfaceKHR surface, int width, int height)
{
	wd->Surface = surface;

	// Check for WSI support
	VkBool32 res;
	//vkGetPhysicalDeviceSurfaceSupportKHR(pDeviceContext->GetPhysicalDevice(), pDeviceContext->GetGraphicsQueueIndex(), wd->Surface, &res);
	vkGetPhysicalDeviceSurfaceSupportKHR(g_PhysicalDevice, g_QueueFamily, wd->Surface, &res);
	if (res != VK_TRUE)
	{
		fprintf(stderr, "Error no WSI support on physical device 0\n");
		exit(-1);
	}

	// Select Surface Format
	const VkFormat requestSurfaceImageFormat[] = {VK_FORMAT_B8G8R8A8_UNORM, VK_FORMAT_R8G8B8A8_UNORM, VK_FORMAT_B8G8R8_UNORM, VK_FORMAT_R8G8B8_UNORM};
	const VkColorSpaceKHR requestSurfaceColorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;
	//wd->SurfaceFormat = ImGui_ImplVulkanH_SelectSurfaceFormat(pDeviceContext->GetPhysicalDevice(), wd->Surface, requestSurfaceImageFormat, (size_t)IM_ARRAYSIZE(requestSurfaceImageFormat), requestSurfaceColorSpace);
	wd->SurfaceFormat = ImGui_ImplVulkanH_SelectSurfaceFormat(g_PhysicalDevice, wd->Surface, requestSurfaceImageFormat, (size_t)IM_ARRAYSIZE(requestSurfaceImageFormat), requestSurfaceColorSpace);

	// Select Present Mode
#ifdef IMGUI_UNLIMITED_FRAME_RATE
	VkPresentModeKHR present_modes[] = {VK_PRESENT_MODE_MAILBOX_KHR, VK_PRESENT_MODE_IMMEDIATE_KHR, VK_PRESENT_MODE_FIFO_KHR};
#else
	VkPresentModeKHR present_modes[] = {VK_PRESENT_MODE_FIFO_KHR};
#endif
	//wd->PresentMode = ImGui_ImplVulkanH_SelectPresentMode(pDeviceContext->GetPhysicalDevice(), wd->Surface, &present_modes[0], IM_ARRAYSIZE(present_modes));
	wd->PresentMode = ImGui_ImplVulkanH_SelectPresentMode(g_PhysicalDevice, wd->Surface, &present_modes[0], IM_ARRAYSIZE(present_modes));
	//printf("[vulkan] Selected PresentMode = %d\n", wd->PresentMode);

	// TODO: IMPORTANT:
	// Setting up the swapchain is a problem. Have a look and see if I actually need to do this or if I can use the existing swapchain.
	// Very important - don't mix the swapchains up! I'm passing one in and this is creating it's own as well.

	// Create SwapChain, RenderPass, Framebuffer, etc.
	//IM_ASSERT(g_MinImageCount >= 2);
	//ImGui_ImplVulkanH_CreateOrResizeWindow(pDeviceContext->GetInstance(), pDeviceContext->GetPhysicalDevice(), pDeviceContext->GetLogicalDevice(),
	//	wd, pDeviceContext->GetGraphicsQueueIndex(), nullptr, width, height, pSwapchain->GetMinImageCount());
	ImGui_ImplVulkanH_CreateOrResizeWindow(g_Instance, g_PhysicalDevice, g_Device, &g_MainWindowData, g_QueueFamily, nullptr, width, height, g_MinImageCount);
}


int main()
{
	bool showDemoWindow = true;
	VkDescriptorPool imGuiDescriptorPool {VK_NULL_HANDLE};

	try
	{
		std::shared_ptr<Jettison::Renderer::Window> pWindow = std::make_shared<Jettison::Renderer::Window>();
		std::shared_ptr<Jettison::Renderer::DeviceContext> pDeviceContext = std::make_shared<Jettison::Renderer::DeviceContext>(pWindow);
		std::shared_ptr<Jettison::Renderer::Swapchain> pSwapchain = std::make_shared<Jettison::Renderer::Swapchain>(pDeviceContext);
		std::shared_ptr<Jettison::Renderer::Pipeline> pPipeline = std::make_shared<Jettison::Renderer::Pipeline>(pDeviceContext, pSwapchain);
		std::shared_ptr<Jettison::Renderer::Renderer> pRenderer = std::make_shared<Jettison::Renderer::Renderer>(pDeviceContext, pWindow, pSwapchain, pPipeline);

		pWindow->Init();
		pDeviceContext->Init();
		pSwapchain->Init();
		pPipeline->Init();
		pRenderer->Init();

		Jettison::Renderer::Model model {pDeviceContext};
		model.LoadModel();

		// Setup Vulkan
		if (!glfwVulkanSupported())
		{
			printf("GLFW: Vulkan Not Supported\n");
			return 1;
		}
		uint32_t extensions_count = 0;
		const char** extensions = glfwGetRequiredInstanceExtensions(&extensions_count);
		SetupVulkan(extensions, extensions_count);

		// Create Window Surface
		VkSurfaceKHR surface;
		//VkResult err = glfwCreateWindowSurface(pDeviceContext->GetInstance(), pWindow->GetGLFWWindow(), nullptr, &surface);
		VkResult err = glfwCreateWindowSurface(g_Instance, pWindow->GetGLFWWindow(), nullptr, &surface);
		CheckVkResult(err);

		// Create Framebuffers
		int w, h;
		glfwGetFramebufferSize(pWindow->GetGLFWWindow(), &w, &h);
		ImGui_ImplVulkanH_Window* wd = &g_MainWindowData;
		//SetupVulkanWindow(pDeviceContext, pSwapchain, wd, surface, w, h);
		SetupVulkanWindow(wd, surface, w, h);

		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

		// Setup Dear ImGui style
		ImGui::StyleColorsDark();

		VkRenderPass imGuiRenderPass {VK_NULL_HANDLE};

		ImGuiCreateRenderPass(pDeviceContext, pSwapchain, imGuiRenderPass);
		//ImGuiCreateDescriptorPool(pDeviceContext, imGuiDescriptorPool);

		// Setup Platform/Renderer bindings
		ImGui_ImplGlfw_InitForVulkan(pWindow->GetGLFWWindow(), true);
		ImGui_ImplVulkan_InitInfo init_info {};
		init_info.Instance = pDeviceContext->GetInstance();
		init_info.PhysicalDevice = pDeviceContext->GetPhysicalDevice();
		init_info.Device = pDeviceContext->GetLogicalDevice();
		init_info.QueueFamily = pDeviceContext->GetGraphicsQueueIndex();
		init_info.Queue = pDeviceContext->GetGraphicsQueue();
		init_info.PipelineCache = VK_NULL_HANDLE;
		init_info.DescriptorPool = imGuiDescriptorPool;
		init_info.Allocator = nullptr;
		init_info.MinImageCount = pSwapchain->GetMinImageCount();
		init_info.ImageCount = pSwapchain->GetImageCount();
		init_info.CheckVkResultFn = CheckVkResult;
		ImGui_ImplVulkan_Init(&init_info, imGuiRenderPass);
		//ImGui_ImplVulkan_Init(&init_info, wd->RenderPass);

		int width;
		int height;

		glfwGetFramebufferSize(pWindow->GetGLFWWindow(), &width, &height);
		if (width > 0 && height > 0)
		{
			ImGui_ImplVulkan_SetMinImageCount(pSwapchain->GetMinImageCount());

			//ImGui_ImplVulkanH_CreateOrResizeWindow(pDeviceContext->GetInstance(), pDeviceContext->GetPhysicalDevice(), pDeviceContext->GetLogicalDevice(),
			//	wd, pDeviceContext->GetGraphicsQueueIndex(), nullptr, width, height, pSwapchain->GetMinImageCount());
			ImGui_ImplVulkanH_CreateOrResizeWindow(g_Instance, g_PhysicalDevice, g_Device, &g_MainWindowData, g_QueueFamily, nullptr, width, height, g_MinImageCount);
		}

		ImGuiInitFontTexture(pDeviceContext);

		while (!glfwWindowShouldClose(pWindow->GetGLFWWindow()))
		{
			glfwPollEvents();

			if (pWindow->HasBeenResized())
			{
				int width;
				int height;

				glfwGetFramebufferSize(pWindow->GetGLFWWindow(), &width, &height);
				if (width > 0 && height > 0)
				{
					ImGui_ImplVulkan_SetMinImageCount(pSwapchain->GetMinImageCount());

					//ImGui_ImplVulkanH_CreateOrResizeWindow(pDeviceContext->GetInstance(), pDeviceContext->GetPhysicalDevice(), pDeviceContext->GetLogicalDevice(),
					//	wd, pDeviceContext->GetGraphicsQueueIndex(), nullptr, width, height, pSwapchain->GetMinImageCount());
					ImGui_ImplVulkanH_CreateOrResizeWindow(g_Instance, g_PhysicalDevice, g_Device, &g_MainWindowData, g_QueueFamily, nullptr, width, height, g_MinImageCount);
				}
			}

			// Start the Dear ImGui frame
			ImGui_ImplVulkan_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();

			// Show the demo window if requested.
			if (showDemoWindow)
				ImGui::ShowDemoWindow(&showDemoWindow);

			// HACK: We need to add the model command buffers to the pipeline.
			pPipeline->CreateCommandBuffers(model);

			pRenderer->DrawFrame();

			// ImGui Rendering.
			ImGui::Render();
			ImDrawData* main_draw_data = ImGui::GetDrawData();
			const bool main_is_minimized = (main_draw_data->DisplaySize.x <= 0.0f || main_draw_data->DisplaySize.y <= 0.0f);

			//memcpy(&wd->ClearValue.color.float32[0], &clear_color, 4 * sizeof(float));
			if (!main_is_minimized)
				FrameRender(pDeviceContext, wd, main_draw_data);

			// Update and Render additional Platform Windows
			//if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
			//{
			//	ImGui::UpdatePlatformWindows();
			//	ImGui::RenderPlatformWindowsDefault();
			//}

			// Present Main Platform Window
			if (!main_is_minimized)
				FramePresent(pDeviceContext, wd);
		}

		pDeviceContext->WaitIdle();

		// ImGui shutdown.
		ImGui_ImplVulkan_Shutdown();

		model.Destroy();
		pRenderer->Destroy();
		pPipeline->Destroy();
		pSwapchain->Destroy();
		pDeviceContext->Destroy();
		pWindow->Destroy();

	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		return EXIT_FAILURE;
	}

	return 0;
}