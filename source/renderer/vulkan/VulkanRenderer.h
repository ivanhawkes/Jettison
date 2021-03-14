#pragma once

#include <vulkan/vulkan.h>

// GL Math.
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <../glm/glm/glm.hpp>
#include <../glm/glm/gtc/matrix_transform.hpp>
#include <../glm/glm/gtx/hash.hpp>

#include <iostream>
#include <cstdlib>
#include <stdexcept>
#include <vector>
#include <optional>
#include <set>
#include <cstdint>
#include <algorithm>
#include <fstream>
#include <array>
#include <chrono>
#include <unordered_map>


// Forward declarations.
struct GLFWwindow;


constexpr uint32_t kWindowWidth = 1920;
constexpr uint32_t kWindowHeight = 1080;

const ::std::string kModelPath = "models/viking_room.wobj";
const ::std::string kTexturePath = "textures/viking_room.png";

constexpr int kMaxFramesInFlight = 2;


const ::std::vector<const char*> validationLayers = {
	"VK_LAYER_KHRONOS_validation"
};

const ::std::vector<const char*> deviceExtensions = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

#ifdef NDEBUG
constexpr bool enableValidationLayers = false;
#else
constexpr bool enableValidationLayers = true;
#endif // NDEBUG


//namespace Jettison::Renderer
//{

struct QueueFamilyIndices
{
	::std::optional<uint32_t> graphicsFamily;
	::std::optional<uint32_t> presentFamily;

	bool isComplete()
	{
		return graphicsFamily.has_value() && presentFamily.has_value();
	}
};


struct SwapChainSupportDetails
{
	VkSurfaceCapabilitiesKHR capabilities = {};
	::std::vector<VkSurfaceFormatKHR> formats;
	::std::vector<VkPresentModeKHR> presentModes;
};


struct Vertex
{
	glm::vec3 pos;
	glm::vec3 color;
	glm::vec2 texCoord;

	static VkVertexInputBindingDescription getBindingDescription()
	{
		VkVertexInputBindingDescription bindingDescription {};
		bindingDescription.binding = 0;
		bindingDescription.stride = sizeof(Vertex);
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		return bindingDescription;
	}


	static ::std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions()
	{
		::std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions {};
		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[0].offset = offsetof(Vertex, pos);

		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[1].offset = offsetof(Vertex, color);

		attributeDescriptions[2].binding = 0;
		attributeDescriptions[2].location = 2;
		attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
		attributeDescriptions[2].offset = offsetof(Vertex, texCoord);

		return attributeDescriptions;
	}


	bool operator==(const Vertex& other) const
	{
		return pos == other.pos && color == other.color && texCoord == other.texCoord;
	}
};


namespace std
{
template<> struct hash<Vertex>
{
	size_t operator()(const Vertex& vertex) const
	{
		return ((hash<glm::vec3>()(vertex.pos) ^
			(hash<glm::vec3>()(vertex.color) << 1)) >> 1) ^
			(hash<glm::vec2>()(vertex.texCoord) << 1);
	}
};
}

struct UniformBufferObject
{
	alignas(16) glm::mat4 model;
	alignas(16) glm::mat4 view;
	alignas(16) glm::mat4 proj;
};


static void check_vk_result(VkResult err)
{
	if (err == 0)
		return;
	fprintf(stderr, "[vulkan] Error: VkResult = %d\n", err);
	if (err < 0)
		abort();
}


class RenderApp {
public:
	void run();

private:
	void initWindow();

	void initVulkan();


	void mainLoop();


	void cleanup();

	void createInstance();

	void createSurface();

	void pickPhysicalDevice();

	void createLogicalDevice();

	void createSwapChain();

	void createImageViews();

	void createRenderPass();

	void createDescriptorSetLayout();

	void createGraphicsPipeline();

	void createFramebuffers();

	void createCommandPool();

	void createColorResources();

	void createDepthResources();

	void createTextureImage();

	void createTextureImageView();

	void createTextureSampler();

	void loadModel();

	void createVertexBuffer();

	void createIndexBuffer();

	void createUniformBuffers();

	void createDescriptorPool();

	void createDescriptorSets();

	void createCommandBuffers();

	void createSyncObjects();

	void drawFrame();

	void updateUniformBuffer(uint32_t currentImage);

	void recreateSwapChain();


	void cleanupSwapChain();

	VkShaderModule createShaderModule(const ::std::vector<char>& code);

	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const ::std::vector<VkSurfaceFormatKHR>& availableFormats);

	VkPresentModeKHR chooseSwapPresentMode(const ::std::vector<VkPresentModeKHR>& availablePresentModes);

	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

	bool isDeviceSuitable(VkPhysicalDevice m_device);

	QueueFamilyIndices findQueueFamilies(VkPhysicalDevice m_device);

	SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice m_device);

	bool checkValidationLayerSupport();

	bool checkDeviceExtensionSupport(VkPhysicalDevice m_device);

	uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

	void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);

	void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

	void generateMipmaps(VkImage image, VkFormat imageFormat, int32_t texWidth, int32_t texHeight, uint32_t mipLevels);

	void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels);

	void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);

	VkCommandBuffer beginSingleTimeCommands();

	void endSingleTimeCommands(VkCommandBuffer commandBuffer);

	void createImage(uint32_t width, uint32_t height, uint32_t mipLevels, VkSampleCountFlagBits numSamples,
		VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties,
		VkImage& image, VkDeviceMemory& imageMemory);

	VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels);

	VkFormat findDepthFormat();

	bool hasStencilComponent(VkFormat format);

	VkFormat findSupportedFormat(const ::std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);

	VkSampleCountFlagBits getMaxUsableSampleCount();

	static ::std::vector<char> readFile(const ::std::string& filename);

	static void framebufferResizeCallback(GLFWwindow* m_window, int width, int height);

	// member variables
	GLFWwindow* m_window = nullptr;
	VkInstance m_instance = VK_NULL_HANDLE;
	VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
	VkDevice m_device = VK_NULL_HANDLE;

	VkQueue m_graphicsQueue = VK_NULL_HANDLE;
	VkQueue m_presentQueue = VK_NULL_HANDLE;
	VkSurfaceKHR m_surface = VK_NULL_HANDLE;
	QueueFamilyIndices m_indicies;

	VkSwapchainKHR m_swapChain = VK_NULL_HANDLE;
	::std::vector<VkImage> m_swapChainImages = {};
	VkFormat m_swapChainImageFormat = {};
	VkExtent2D m_swapChainExtent = {};
	::std::vector<VkImageView> m_swapChainImageViews = {};
	uint32_t m_swapChainImageCount {0};

	VkRenderPass m_renderPass = VK_NULL_HANDLE;

	VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
	VkPipeline graphicsPipeline = VK_NULL_HANDLE;
	::std::vector<VkFramebuffer> swapChainFramebuffers = {};
	VkCommandPool m_commandPool = VK_NULL_HANDLE;
	::std::vector<VkCommandBuffer> m_commandBuffers = {};

	::std::vector<VkSemaphore> imageAvailableSemaphores = {};
	::std::vector<VkSemaphore> renderFinishedSemaphores = {};

	::std::vector<VkFence> inFlightFences = {};
	::std::vector<VkFence> imagesInFlight = {};

	size_t currentFrame = 0;
	bool framebufferResized = false;

	::std::vector<Vertex> vertices = {};
	::std::vector<uint32_t> indices = {};

	VkBuffer vertexBuffer = VK_NULL_HANDLE;
	VkDeviceMemory vertexBufferMemory = VK_NULL_HANDLE;
	VkBuffer indexBuffer = VK_NULL_HANDLE;
	VkDeviceMemory indexBufferMemory = VK_NULL_HANDLE;
	::std::vector<VkBuffer> uniformBuffers = {};
	::std::vector<VkDeviceMemory> uniformBuffersMemory = {};

	VkDescriptorPool m_descriptorPool = VK_NULL_HANDLE;
	::std::vector<VkDescriptorSet> m_descriptorSets = {};
	VkDescriptorSetLayout descriptorSetLayout = VK_NULL_HANDLE;

	uint32_t mipLevels = 0;
	VkImage textureImage = VK_NULL_HANDLE;
	VkDeviceMemory textureImageMemory = VK_NULL_HANDLE;
	VkImageView textureImageView = VK_NULL_HANDLE;
	VkSampler textureSampler = VK_NULL_HANDLE;
	VkImage depthImage = VK_NULL_HANDLE;
	VkDeviceMemory depthImageMemory = VK_NULL_HANDLE;
	VkImageView depthImageView = VK_NULL_HANDLE;
	VkSampleCountFlagBits msaaSamples = VK_SAMPLE_COUNT_1_BIT;
	VkImage colorImage = VK_NULL_HANDLE;
	VkDeviceMemory colorImageMemory = nullptr;
	VkImageView colorImageView = VK_NULL_HANDLE;

	bool show_demo_window = true;
};


//}