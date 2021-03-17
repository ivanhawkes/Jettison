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

#include <array>

#include "VulkanDevice.h"


// Forward declarations.
struct GLFWwindow;


namespace Jettison::Renderer
{
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


	static std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions()
	{
		std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions {};
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
}


namespace std
{
template<> struct hash<Jettison::Renderer::Vertex>
{
	size_t operator()(const Jettison::Renderer::Vertex& vertex) const
	{
		return ((hash<glm::vec3>()(vertex.pos) ^
			(hash<glm::vec3>()(vertex.color) << 1)) >> 1) ^
			(hash<glm::vec2>()(vertex.texCoord) << 1);
	}
};
}


namespace Jettison::Renderer
{
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


class VulkanRenderer {
public:
	void DrawFrame();

	void InitWindow();

	void InitVulkan();

	void Cleanup();

	GLFWwindow* GetWindow() const { return m_window; }

	void WaitIdle() { m_vulkanDevice.WaitIdle(); }


private:
	void CreateSwapChain();

	void CreateImageViews();

	void CreateRenderPass();

	void CreateDescriptorSetLayout();

	void CreateGraphicsPipeline();

	void CreateFramebuffers();

	void CreateCommandPool();

	void CreateColorResources();

	void CreateDepthResources();

	void CreateTextureImage();

	void CreateTextureImageView();

	void CreateTextureSampler();

	void LoadModel();

	void CreateVertexBuffer();

	void CreateIndexBuffer();

	void CreateUniformBuffers();

	void CreateDescriptorPool();

	void CreateDescriptorSets();

	void CreateCommandBuffers();

	void CreateSyncObjects();

	void UpdateUniformBuffer(uint32_t currentImage);

	void RecreateSwapChain();

	void CleanupSwapChain();

	VkShaderModule CreateShaderModule(const std::vector<char>& code);

	VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);

	VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);

	VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

	uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

	void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);

	void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

	void GenerateMipmaps(VkImage image, VkFormat imageFormat, int32_t texWidth, int32_t texHeight, uint32_t mipLevels);

	void TransitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels);

	void CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);

	VkCommandBuffer BeginSingleTimeCommands();

	void EndSingleTimeCommands(VkCommandBuffer commandBuffer);

	void CreateImage(uint32_t width, uint32_t height, uint32_t mipLevels, VkSampleCountFlagBits numSamples,
		VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties,
		VkImage& image, VkDeviceMemory& imageMemory);

	VkImageView CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels);

	VkFormat FindDepthFormat();

	bool HasStencilComponent(VkFormat format);

	VkFormat FindSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);

	VkSampleCountFlagBits GetMaxUsableSampleCount();

	static std::vector<char> ReadFile(const std::string& filename);

	static void FramebufferResizeCallback(GLFWwindow* m_window, int width, int height);

	// Vulkan device.
	VulkanDevice m_vulkanDevice {};

	GLFWwindow* m_window {nullptr};

	VkSwapchainKHR m_swapChain {VK_NULL_HANDLE};

	std::vector<VkImage> m_swapChainImages {};
	VkFormat m_swapChainImageFormat;
	VkExtent2D m_swapChainExtent;
	std::vector<VkImageView> m_swapChainImageViews {};
	uint32_t m_swapChainImageCount {0};

	VkRenderPass m_renderPass {VK_NULL_HANDLE};

	VkPipelineLayout m_pipelineLayout {VK_NULL_HANDLE};
	VkPipeline m_graphicsPipeline {VK_NULL_HANDLE};
	std::vector<VkFramebuffer> m_swapChainFramebuffers {};
	VkCommandPool m_commandPool {VK_NULL_HANDLE};
	std::vector<VkCommandBuffer> m_commandBuffers {};

	std::vector<VkSemaphore> m_imageAvailableSemaphores {};
	std::vector<VkSemaphore> m_renderFinishedSemaphores {};

	std::vector<VkFence> m_inFlightFences {};
	std::vector<VkFence> m_imagesInFlight {};

	size_t m_currentFrame {0};
	bool m_framebufferResized {false};

	std::vector<Vertex> m_vertices {};
	std::vector<uint32_t> m_indices {};

	VkBuffer m_vertexBuffer {VK_NULL_HANDLE};
	VkDeviceMemory m_vertexBufferMemory {VK_NULL_HANDLE};
	VkBuffer m_indexBuffer {VK_NULL_HANDLE};
	VkDeviceMemory m_indexBufferMemory {VK_NULL_HANDLE};
	std::vector<VkBuffer> m_uniformBuffers {};
	std::vector<VkDeviceMemory> m_uniformBuffersMemory {};

	VkDescriptorPool m_descriptorPool {VK_NULL_HANDLE};
	std::vector<VkDescriptorSet> m_descriptorSets {};
	VkDescriptorSetLayout m_descriptorSetLayout {VK_NULL_HANDLE};

	uint32_t m_mipLevels {0};
	VkImage m_textureImage {VK_NULL_HANDLE};
	VkDeviceMemory m_textureImageMemory {VK_NULL_HANDLE};
	VkImageView m_textureImageView {VK_NULL_HANDLE};
	VkSampler m_textureSampler {VK_NULL_HANDLE};
	VkImage m_depthImage {VK_NULL_HANDLE};
	VkDeviceMemory m_depthImageMemory {VK_NULL_HANDLE};
	VkImageView m_depthImageView {VK_NULL_HANDLE};
	VkSampleCountFlagBits m_msaaSamples {VK_SAMPLE_COUNT_1_BIT};
	VkImage m_colorImage {VK_NULL_HANDLE};
	VkDeviceMemory m_colorImageMemory {nullptr};
	VkImageView m_colorImageView {VK_NULL_HANDLE};

	bool m_show_demo_window {true};
};
}