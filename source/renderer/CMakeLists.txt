cmake_minimum_required (VERSION 3.18)

project("renderer" VERSION 0.1.0)

# Force the compiler to c++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(Renderer SHARED)
set_property(TARGET Renderer PROPERTY FOLDER "Renderer")

target_link_libraries(Renderer PUBLIC Vulkan::Vulkan)

# GL math library.
target_link_libraries(Renderer PUBLIC glm)

# GLFW.
#target_link_libraries(Renderer PUBLIC glfw)

# GLFW.
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory("${CMAKE_SOURCE_DIR}/external/glfw")


# STB.
target_link_libraries(Renderer PUBLIC stb)

# Tiny Object Loader.
target_link_libraries(Renderer PUBLIC tiny_obj_loader)


target_sources(Renderer PUBLIC
    # Interface.
    IRenderer.h

    # ImGUI's implementation.
    imgui/ImGuiRenderer.cpp
    imgui/ImGuiRenderer.h
    )

target_sources(Renderer PUBLIC
    # ImGUI's implementations.
    imgui/imgui_impl_glfw.cpp
    imgui/imgui_impl_glfw.h
    imgui/imgui_impl_vulkan.cpp
    imgui/imgui_impl_vulkan.h

    # Base files.
    imgui/imconfig.h
    imgui/imgui.cpp
    imgui/imgui.h
    imgui/imgui_demo.cpp
    imgui/imgui_draw.cpp
    imgui/imgui_internal.h
    imgui/imgui_widgets.cpp
    imgui/imstb_rectpack.h
    imgui/imgui_tables.cpp
    imgui/imstb_textedit.h
    imgui/imstb_truetype.h
    )

# Move the targets into a solution folder.
#set_property(TARGET Renderer PROPERTY FOLDER "Renderer/Vulkan")

target_sources(Renderer PUBLIC
    # Vulkan's implementation.
    vulkan/VulkanRenderer.cpp
    vulkan/VulkanRenderer.h
    )

