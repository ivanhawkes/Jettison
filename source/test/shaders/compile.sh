#!/bin/sh
glslc.exe shader.vert -o shader.vert.spv
glslc.exe shader.frag -o shader.frag.spv
glslc.exe imgui.vert -o imgui.vert.spv
glslc.exe imgui.frag -o imgui.frag.spv
