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


namespace Jettison::Renderer
{
class VulkanPipeline
{
public:
	// Disable copying.
	VulkanPipeline() = default;
	VulkanPipeline(const VulkanPipeline&) = delete;
	VulkanPipeline& operator=(const VulkanPipeline&) = delete;
};
}
