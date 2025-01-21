cmake_minimum_required(VERSION 3.16)
project(VoxelEngine)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Export compile commands for SonarQube
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_COMPILER_LAUNCHER ccache)

if(WIN32)
  set(CMAKE_TOOLCHAIN_FILE "${CMAKE_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake")
else()
  include_directories(/usr/include)
  link_directories(/usr/lib)
endif()

set(OpenGL_GL_PREFERENCE GLVND)
find_package(OpenGL REQUIRED)
find_package(glfw3 CONFIG REQUIRED)

# Add ImGui source files
set(IMGUI_DIR external/imgui)
set(IMGUI_SOURCES
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_demo.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)

# Add source files
set(SOURCES
    external/glad/src/glad.c
    external/stb_image/src/stb_image.cpp
    src/EntryPoint.cpp
    src/Application.cpp
    src/Window/OpenGLWindow.cpp
    src/Renderer/Buffer.cpp
    src/Renderer/OpenGLBuffer.cpp
    src/Renderer/Renderer.cpp
    src/Shader/Shader.cpp
    src/ImGui/ImGuiLayer.cpp
    src/Renderer/VertexArray.cpp
    src/Renderer/OpenGLVertexArray.cpp
    src/Camera/PerspectiveCamera.cpp
    src/Renderer/Material.cpp
    src/Renderer/Texture.cpp
    src/Debug/Profiler.cpp
    src/Threading/ThreadPool.cpp
    src/VoxelTerrain.cpp
    src/Noise/VoidNoise/VoidNoise.cpp
    src/Noise/PerlinNoise/PerlinNoise.cpp
    src/Input/InputSystem.cpp
    src/TerrainSystem/TerrainSystem.cpp
    src/UI/ImGuiOverlay.cpp
    src/UI/ImGuiFlameGraph.cpp
    src/VoxelChunk.cpp
    src/Core/FPSCounter.cpp
    src/Shader/ShaderHotReload.cpp
    src/Noise/SimplexNoise/SimplexNoise.cpp
    src/Noise/SimplexNoise/SimplexNoise.h
    src/Noise/ValueNoise/ValueNoise.cpp
    src/Noise/ValueNoise/ValueNoise.h
    ${IMGUI_SOURCES}
)

add_executable(voxel-engine ${SOURCES})

target_include_directories(voxel-engine PRIVATE external/glad/include)

target_link_libraries(voxel-engine PRIVATE glfw OpenGL::GL)

target_include_directories(voxel-engine PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
    ${CMAKE_SOURCE_DIR}/external/stb_image/src
    ${CMAKE_SOURCE_DIR}/external/json
)

# Enable precompiled headers - specify C++ explicitly
target_precompile_headers(voxel-engine
  PRIVATE
    $<INSTALL_INTERFACE:pch.h>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/pch.h>
)

# Force C++ compilation for PCH
set_source_files_properties(
  ${CMAKE_CURRENT_SOURCE_DIR}/src/pch.h
  PROPERTIES
  LANGUAGE CXX
)

# Ensure PCH is used for all C++ files
set_target_properties(voxel-engine PROPERTIES
  COMPILE_FLAGS "-include pch.h"
)