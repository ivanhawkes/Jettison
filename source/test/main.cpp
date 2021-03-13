#include <vulkan/VulkanRenderer.h>

#include <iostream>
#include <stdexcept>


int main() {
	Jettison::Renderer::HelloWorldApp app;

	try
	{
		app.run();
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		return EXIT_FAILURE;
	}

	return 0;
}