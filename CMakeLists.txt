cmake_minimum_required(VERSION 3.25)

project(SBPatchHelper VERSION 0.1.1 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(json
    GIT_REPOSITORY https://github.com/nlohmann/json
    GIT_TAG v3.11.3
)

FetchContent_MakeAvailable(json)

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
