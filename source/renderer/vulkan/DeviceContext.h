#pragma once

#include <vulkan/vulkan.h>

// STD.
#include <memory>
#include <optional>
#include <vector>

#include "Window.h"


namespace Jettison::Renderer
{
struct QueueFamilyIndices
{
	std::optional<uint32_t> graphicsFamily;
	std::optional<uint32_t> presentFamily;

	bool isComplete()
	{
		return graphicsFamily.has_value() && presentFamily.has_value();
	}
};


struct SwapChainSupportDetails
{
	VkSurfaceCapabilitiesKHR capabilities = {};
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;
};


class DeviceContext
{
public:
	DeviceContext(std::shared_ptr<Window> pWindow)
		:m_pWindow {pWindow} {}

	// Disable copying.
	DeviceContext() = default;
	DeviceContext(const DeviceContext&) = delete;
	DeviceContext& operator=(const DeviceContext&) = delete;

	void Init();

	void Destroy();

	void WaitIdle() { vkDeviceWaitIdle(m_logicalDevice); }

	// Wait for the device to return sensible values for the window size after a resize event.
	void WaitOnWindowResized() const;

	inline VkPhysicalDevice GetPhysicalDevice() const { return m_physicalDevice; }

	inline VkDevice GetLogicalDevice() const { return m_logicalDevice; }

	inline VkInstance GetInstance() const { return m_instance; }

	inline VkSampleCountFlagBits GetMsaaSamples() const { return m_msaaSamples; }

	inline VkQueue GetGraphicsQueue() const { return m_graphicsQueue; }

	inline VkQueue GetPresentQueue() const { return m_presentQueue; }

	inline std::shared_ptr<Window> GetWindow() const { return m_pWindow; }

	// Utilities.

	VkFormat FindDepthFormat();

	void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);

	void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

	void CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);

	VkCommandBuffer BeginSingleTimeCommands();

	void EndSingleTimeCommands(VkCommandBuffer commandBuffer);

	void TransitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels);

	void CreateImage(uint32_t width, uint32_t height, uint32_t mipLevels, VkSampleCountFlagBits numSamples,
		VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties,
		VkImage& image, VkDeviceMemory& imageMemory);

	VkImageView CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels);

	VkShaderModule CreateShaderModule(const std::vector<char>& code);

	VkSurfaceKHR GetSurface() const { return m_surface; }

	VkCommandPool GetCommandPool() const { return m_commandPool; }

	SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device);

	QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice m_device);

private:
	bool CheckValidationLayerSupport();

	void CreateInstance();

	void CreateSurface();

	bool CheckDeviceExtensionSupport(VkPhysicalDevice device);

	bool IsDeviceSuitable(VkPhysicalDevice m_device);

	void PickPhysicalDevice();

	void CreateLogicalDevice();

	uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

	bool HasStencilComponent(VkFormat format);

	VkSampleCountFlagBits GetMaxUsableSampleCount();

	VkFormat FindSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);

	void CreateCommandPool();

	std::shared_ptr<Window> m_pWindow {nullptr};

	VkInstance m_instance {VK_NULL_HANDLE};
	VkPhysicalDevice m_physicalDevice {VK_NULL_HANDLE};
	VkDevice m_logicalDevice {VK_NULL_HANDLE};

	VkSampleCountFlagBits m_msaaSamples {VK_SAMPLE_COUNT_1_BIT};

	VkQueue m_graphicsQueue {VK_NULL_HANDLE};
	VkQueue m_presentQueue {VK_NULL_HANDLE};
	VkSurfaceKHR m_surface {VK_NULL_HANDLE};

	VkCommandPool m_commandPool {VK_NULL_HANDLE};
};
}
