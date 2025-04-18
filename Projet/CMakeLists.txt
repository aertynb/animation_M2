cmake_minimum_required(VERSION 3.10)
cmake_policy(SET CMP0048 NEW)

project(anim-project VERSION 0.0.1)

if(NOT DEFINED CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

message(STATUS "Preparing solution with CMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}")

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(IMGUI_DIR imgui-1.74)
set(GLFW_DIR glfw-3.3.1)
set(GLM_DIR glm-0.9.9.7)
set(GLAD_DIR glad)
set(TINYGLTF_DIR tinygltf-bcf2ce586ee8bf2a2a816afa6bfe2f8692ba6ac2)
set(ARGS_DIR args-6.2.2)

# Add GLFW subdirectory, set some options to OFF by default (the user can still enable them by modifying its CMakeCache.txt)
option(GLFW_BUILD_DOCS OFF)
option(GLFW_BUILD_TESTS OFF)
option(GLFW_BUILD_EXAMPLES OFF)
add_subdirectory(third-party/${GLFW_DIR})

if(${CMAKE_VERSION} VERSION_LESS "3.10.0")
    set(OpenGL_GL_PREFERENCE LEGACY)
else()
    set(OpenGL_GL_PREFERENCE GLVND)
endif()

find_package(OpenGL REQUIRED)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include(c2ba-glsl-shaders)
include(c2ba-copy-dll-post-build)
include(c2ba-assets-directory)

if(MSVC)
    set(SHADER_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin/\$\(Configuration\)/shaders)
    set(ASSET_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin/\$\(Configuration\)/assets)
else()
    set(SHADER_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin/shaders)
    set(ASSET_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin/assets)
endif()

set(
    IMGUI_SRC_FILES
    third-party/${IMGUI_DIR}/imgui.cpp
    third-party/${IMGUI_DIR}/imgui_demo.cpp
    third-party/${IMGUI_DIR}/imgui_draw.cpp
    third-party/${IMGUI_DIR}/imgui_widgets.cpp
    third-party/${IMGUI_DIR}/examples/imgui_impl_opengl3.cpp
    third-party/${IMGUI_DIR}/examples/imgui_impl_glfw.cpp
)

set(
    THIRD_PARTY_SRC_FILES
    ${IMGUI_SRC_FILES}
    third-party/${GLAD_DIR}/src/glad.c
)

set(
    LIBRARIES
    ${OPENGL_LIBRARIES}
    glfw
)

source_group("glsl" REGULAR_EXPRESSION ".*/*.glsl")
source_group("third-party" REGULAR_EXPRESSION "third-party/*.*")

set(APP gltf-viewer)
set(SRC_DIR ${CMAKE_SOURCE_DIR}/src)

file(
    GLOB_RECURSE
    SRC_FILES
    ${SRC_DIR}/*
)

add_executable(
    ${APP}
    ${SRC_FILES}
    ${THIRD_PARTY_SRC_FILES}
)

target_include_directories(
    ${APP}
    PUBLIC
    ${OPENGL_INCLUDE_DIRS}
    third-party/${GLFW_DIR}/include
    third-party/${GLM_DIR}
    third-party/${GLAD_DIR}/include
    third-party/${IMGUI_DIR}
    third-party/${IMGUI_DIR}/examples/
    third-party/${TINYGLTF_DIR}/include
    third-party/${ARGS_DIR}
    lib/include
)

target_compile_definitions(
    ${APP}
    PUBLIC
    IMGUI_IMPL_OPENGL_LOADER_GLAD
    GLM_ENABLE_EXPERIMENTAL
)

set_property(TARGET ${APP} PROPERTY CXX_STANDARD 17)

target_link_libraries(
    ${APP}
    ${LIBRARIES}
)

install(
    TARGETS ${APP}
    DESTINATION .
)

c2ba_add_shader_directory(${SRC_DIR}/shaders ${SHADER_OUTPUT_PATH})
c2ba_add_assets_directory(${SRC_DIR}/assets ${ASSET_OUTPUT_PATH})

install(
    DIRECTORY ${SRC_DIR}/shaders/
    DESTINATION shaders/
    FILES_MATCHING PATTERN "*.glsl"
)

if(EXISTS ${SRC_DIR}/assets/)
    install(
        DIRECTORY ${SRC_DIR}/assets/
        DESTINATION assets/
    )
endif()
