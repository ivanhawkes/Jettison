@echo off

set argone=%1
if defined argone cd %1

REM TEST
glslc.exe shader.vert -o shader.vert.spv
glslc.exe shader.frag -o shader.frag.spv

REM IMGUI
glslc.exe imgui.vert -o imgui.vert.spv
glslc.exe imgui.frag -o imgui.frag.spv
