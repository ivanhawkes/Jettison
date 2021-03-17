#include <vulkan/VulkanRenderer.h>

// GLFW / Vulkan.
#define GLFW_INCLUDE_VULKAN
#include <../glfw/include/GLFW/glfw3.h>

#include <iostream>
#include <stdexcept>


int main() {
	Jettison::Renderer::VulkanRenderer vulkanRenderer;

	try
	{
		vulkanRenderer.InitWindow();
		vulkanRenderer.InitVulkan();

		// TODO: InitImGui();

		while (!glfwWindowShouldClose(vulkanRenderer.GetWindow()))
		{
			glfwPollEvents();

			// TODO: ImGui before.

			vulkanRenderer.DrawFrame();

			// TODO: ImGui after.
		}

		vulkanRenderer.WaitIdle();
		vulkanRenderer.Cleanup();

	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		return EXIT_FAILURE;
	}

	return 0;
}