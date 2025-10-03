# CMakeList.txt : CMake project for EncounterMaster, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

project ("EncounterMaster" C CXX)

# Add source to this project's executable.
add_executable (EncounterMaster 
"EncounterMaster.cpp" "Menu.cpp" "Creature.h" "Creature.cpp" "Download.h" "Download.cpp" "CreatureList.h" "CreatureList.cpp"
"imgui/imgui.cpp" "imgui/imgui_draw.cpp" "imgui/imgui_demo.cpp" "imgui/imgui_tables.cpp" "imgui/imgui_widgets.cpp" "imgui/backends/imgui_impl_sdlrenderer3.cpp" "imgui/backends/imgui_impl_sdl3.cpp" "CreatureEdit.h" "CreatureEdit.cpp" )

set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")
include("${CMAKE_TOOLCHAIN_FILE}")

find_package(SDL3 CONFIG REQUIRED)
find_package(SDL3_image CONFIG REQUIRED)
find_package(CURL REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)

include_directories(${CURL_INCLUDE_DIRS})

target_link_libraries(EncounterMaster PRIVATE 
	SDL3::SDL3
	SDL3_image::SDL3_image-shared
	CURL::libcurl
	nlohmann_json::nlohmann_json
)