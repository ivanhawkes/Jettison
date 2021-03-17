#pragma once

#include <vulkan/vulkan.h>
#include <optional>
#include <vector>


struct GLFWwindow;


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


class VulkanDevice
{
public:
	void CreateInstance();
	bool CheckValidationLayerSupport();
	void CreateSurface(GLFWwindow* window);
	void PickPhysicalDevice();
	void CreateLogicalDevice();

	void WaitIdle() { vkDeviceWaitIdle(m_device); }

	QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice m_device);
	bool CheckDeviceExtensionSupport(VkPhysicalDevice m_device);
	bool IsDeviceSuitable(VkPhysicalDevice m_device);
	SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice m_device);

	VkInstance GetInstance() const { return m_instance; }
	VkSurfaceKHR GetSurface() const { return m_surface; }
	VkPhysicalDevice GetPhysicalDevice() const { return m_physicalDevice; }
	VkDevice GetDevice() const { return m_device; }
	VkSampleCountFlagBits GetMsaaSamples() const { return m_msaaSamples; }
	VkQueue GetGraphicsQueue() const { return m_graphicsQueue; }
	VkQueue GetPresentQueue() const { return m_presentQueue; }

private:
	VkSampleCountFlagBits GetMaxUsableSampleCount();

	VkInstance m_instance {VK_NULL_HANDLE};
	VkSurfaceKHR m_surface {VK_NULL_HANDLE};
	VkPhysicalDevice m_physicalDevice {VK_NULL_HANDLE};
	VkDevice m_device {VK_NULL_HANDLE};
	QueueFamilyIndices m_indicies;
	VkSampleCountFlagBits m_msaaSamples {VK_SAMPLE_COUNT_1_BIT};

	VkQueue m_graphicsQueue {VK_NULL_HANDLE};
	VkQueue m_presentQueue {VK_NULL_HANDLE};
};
}
