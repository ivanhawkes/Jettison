target_sources(test PRIVATE
	# ImGUI's implementations.
	imgui_impl_glfw.cpp
	imgui_impl_glfw.h
	imgui_impl_vulkan.cpp
	imgui_impl_vulkan.h

	# Base files.
	imconfig.h
	imgui.cpp
	imgui.h
	imgui_demo.cpp
	imgui_draw.cpp
	imgui_internal.h
	imgui_widgets.cpp
	imstb_rectpack.h
	imgui_tables.cpp
	imstb_textedit.h
	imstb_truetype.h
	)