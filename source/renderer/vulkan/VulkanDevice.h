#pragma once

#include <vulkan/vulkan.h>


namespace Jettison::Renderer
{
class VulkanDevice
{
public:
	void CreateInstance();
	bool CheckValidationLayerSupport();

	VkInstance GetInstance() const { return m_instance; }

private:
	VkInstance m_instance {VK_NULL_HANDLE};
};
}
