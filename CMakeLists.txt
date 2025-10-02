# CMakeList.txt : CMake project for EncounterMaster, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

project ("EncounterMaster" C CXX)

# Add source to this project's executable.
add_executable (EncounterMaster "EncounterMaster.cpp" "imgui/imgui.cpp" "imgui/imgui_draw.cpp" "imgui/imgui_demo.cpp" "imgui/imgui_tables.cpp" "imgui/imgui_widgets.cpp" "imgui/backends/imgui_impl_sdlrenderer3.cpp" "imgui/backends/imgui_impl_sdl3.cpp")

# VCPKG configuration (wouldn't work without manually setting it sorry)
set(CMAKE_TOOLCHAIN_FILE "E:/vcpkg/scripts/buildsystems/vcpkg.cmake")
include("${CMAKE_TOOLCHAIN_FILE}")

find_package(SDL3 CONFIG REQUIRED)
target_link_libraries(EncounterMaster PRIVATE 
	SDL3::SDL3
)