cmake_minimum_required (VERSION 3.18)

# Set the overall project name.
project("test" VERSION 0.1.0)

# Force the compiler to c++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(test main.cpp)

# Move the targets into a solution folder.
set_property(TARGET test PROPERTY FOLDER "Test")

# Find and link Vulkan.
find_package(Vulkan REQUIRED)
target_link_libraries(test PRIVATE Vulkan::Vulkan)

include_directories("external/entt/src")
include_directories("external/imgui")

# Copy shaders, models, textures and fonts.
configure_file("textures/texture.jpg" "textures/texture.jpg" COPYONLY)
configure_file("textures/viking_room.png" "textures/viking_room.png" COPYONLY)

configure_file("models/viking_room.wobj" "models/viking_room.wobj" COPYONLY)

configure_file("../shaders/shader.vert.spv" "shaders/shader.vert.spv" COPYONLY)
configure_file("../shaders/shader.frag.spv" "shaders/shader.frag.spv" COPYONLY)
configure_file("../shaders/imgui.vert.spv" "shaders/imgui.vert.spv" COPYONLY)
configure_file("../shaders/imgui.frag.spv" "shaders/imgui.frag.spv" COPYONLY)
configure_file("../fonts/Cousine-Regular.ttf" "fonts/Cousine-Regular.ttf" COPYONLY)
configure_file("../fonts/DroidSans.ttf" "fonts/DroidSans.ttf" COPYONLY)
configure_file("../fonts/Hack-Bold.ttf" "fonts/Hack-Bold.ttf" COPYONLY)
configure_file("../fonts/Hack-BoldItalic.ttf" "fonts/Hack-BoldItalic.ttf" COPYONLY)
configure_file("../fonts/Hack-Italic.ttf" "fonts/Hack-Italic.ttf" COPYONLY)
configure_file("../fonts/Hack-Regular.ttf" "fonts/Hack-Regular.ttf" COPYONLY)
configure_file("../fonts/Karla-Regular.ttf" "fonts/Karla-Regular.ttf" COPYONLY)
configure_file("../fonts/ProggyClean.ttf" "fonts/ProggyClean.ttf" COPYONLY)
configure_file("../fonts/ProggyTiny.ttf" "fonts/ProggyTiny.ttf" COPYONLY)
configure_file("../fonts/Roboto-Medium.ttf" "fonts/Roboto-Medium.ttf" COPYONLY)
configure_file("../fonts/VeraMono.ttf" "fonts/VeraMono.ttf" COPYONLY)

# GL math library.
target_link_libraries(test PRIVATE glm)

# GLFW.
target_link_libraries(test PRIVATE glfw)

# STB.
target_link_libraries(test PRIVATE stb)

# Tiny Object Loader.
target_link_libraries(test PRIVATE tiny_obj_loader)

# ImGUI
add_subdirectory("imgui")

# Renderer.
add_subdirectory("${CMAKE_SOURCE_DIR}/renderer" build)
