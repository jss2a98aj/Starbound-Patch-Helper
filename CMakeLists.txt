cmake_minimum_required(VERSION 3.28 FATAL_ERROR)

project(SBPatchHelper VERSION 0.1.1 LANGUAGES C CXX)

# Require C++ 20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Used by clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# CPM
file(
	DOWNLOAD
		https://github.com/cpm-cmake/CPM.cmake/releases/download/v0.42.0/CPM.cmake
		${CMAKE_CURRENT_BINARY_DIR}/cmake/CPM.cmake
		EXPECTED_HASH SHA256=2020b4fc42dba44817983e06342e682ecfc3d2f484a581f11cc5731fbe4dce8a
)
include(${CMAKE_CURRENT_BINARY_DIR}/cmake/CPM.cmake)

# Nlohmann JSON 3.12.0
CPMAddPackage(
	URI "gh:nlohmann/json#55f9368"
	OPTIONS "JSON_BuildTests OFF"
)

add_executable(${PROJECT_NAME}
    starbound_patch_helper.cpp
    global_settings.cpp
    json_intermediary_writer.cpp
    json_patch_writer.cpp
    parse_settings.cpp
    patch_style_settings.cpp
    user_interaction_helper.cpp
    utilities.cpp
)
target_link_libraries(${PROJECT_NAME} nlohmann_json::nlohmann_json)

#TODO: Figure out why PROJECT_BINARY_DIR is not the actual folder the binary goes in when building.
add_custom_target(copy_config ALL
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${PROJECT_SOURCE_DIR}/config
        ${PROJECT_BINARY_DIR}/Debug/config
        COMMENT "Copying config into binary directory")
add_dependencies(${PROJECT_NAME} copy_config)

add_custom_target(copy_source_assets ALL
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${PROJECT_SOURCE_DIR}/source_assets
        ${PROJECT_BINARY_DIR}/Debug/source_assets
        COMMENT "Copying source_assets into binary directory")
add_dependencies(${PROJECT_NAME} copy_source_assets)
