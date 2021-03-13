cmake_minimum_required (VERSION 3.18)

project("renderer" VERSION 0.1.0)

# Force the compiler to c++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(Renderer SHARED)

# Move the targets into a solution folder.
set_property(TARGET Renderer PROPERTY FOLDER "Renderer")


target_sources(Renderer PUBLIC
    # Interface.
    IRenderer.h

    # ImGUI's implementation.
    imgui/ImGuiRenderer.cpp
    imgui/ImGuiRenderer.h
    )

# Move the targets into a solution folder.
#set_property(TARGET Renderer PROPERTY FOLDER "Renderer/Vulkan")

target_sources(Renderer PUBLIC
    # Vulkan's implementation.
    vulkan/VulkanRenderer.cpp
    vulkan/VulkanRenderer.h
    )

