@echo off

set argone=%1
if defined argone cd %1

glslc.exe simple-vert.vert
glslc.exe simple-fragment.frag