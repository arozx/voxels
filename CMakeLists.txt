cmake_minimum_required(VERSION 3.16)
project(VoxelEngine)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Export compile commands for SonarQube
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_COMPILER_LAUNCHER ccache)

if(WIN32)
    set(CMAKE_TOOLCHAIN_FILE "$ENV{USERPROFILE}/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
else()
    include_directories(/usr/include)
    link_directories(/usr/lib)
endif()

set(OpenGL_GL_PREFERENCE GLVND)

find_package(OpenGL REQUIRED)
if (NOT OPENGL_FOUND)
    message(FATAL_ERROR "OpenGL not found. Please install OpenGL package.")
endif()

find_package(glfw3 CONFIG REQUIRED)
if (NOT glfw3_FOUND)
    message(FATAL_ERROR "GLFW not found. Please install GLFW3 package.")
endif()

find_package(Lua 5.4 REQUIRED)
if(NOT LUA_FOUND)
    message(FATAL_ERROR "Lua >= 5.4 not found. Please install lua5.4 and lua5.4-dev packages.")
endif()

# Add GLAD
add_library(glad STATIC
    external/glad/src/glad.c
)
target_include_directories(glad PUBLIC
    external/glad/include
)

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

# Engine library sources
set(ENGINE_SOURCES
    src/Application.cpp
    src/Window/OpenGLWindow.cpp
    src/Renderer/Buffer.cpp
    src/Renderer/OpenGLBuffer.cpp
    src/Renderer/Renderer.cpp
    src/Renderer/Renderer2D.cpp
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
    src/Noise/ValueNoise/ValueNoise.cpp
    external/stb_image/src/stb_image.cpp
    src/Scripting/LuaScriptSystem.cpp
    src/Scene/Scene.cpp
    src/Shader/ShaderLibrary.cpp
    src/Shader/ShaderFactory.cpp
    ${IMGUI_SOURCES}
)

# Create engine library
add_library(voxel-engine STATIC ${ENGINE_SOURCES})

target_include_directories(voxel-engine PUBLIC
    ${CMAKE_SOURCE_DIR}/src
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
    ${CMAKE_SOURCE_DIR}/build/vcpkg_installed/x86-windows/include
    ${CMAKE_SOURCE_DIR}/external/stb_image/src
    ${CMAKE_SOURCE_DIR}/external/json
    ${CMAKE_SOURCE_DIR}/external/glad/include
    ${CMAKE_SOURCE_DIR}/build/vcpkg_installed/x86-windows/include
    ${CMAKE_SOURCE_DIR}/external/sol2
)

target_link_libraries(voxel-engine PUBLIC 
    glfw 
    OpenGL::GL
    glad
    ${LUA_LIBRARIES}  # Add Lua libraries
)

# Create sandbox executable
set(SANDBOX_SOURCES
    sandbox/src/SandboxApp.cpp
)

add_executable(sandbox ${SANDBOX_SOURCES})
target_link_libraries(sandbox PRIVATE voxel-engine)

add_custom_command(TARGET sandbox POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/sandbox/assets/scripts
    ${CMAKE_BINARY_DIR}/assets/scripts
)

add_custom_command(TARGET sandbox POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:sandbox>/assets
)

# Enable precompiled headers - specify C++ explicitly
target_precompile_headers(voxel-engine
  PRIVATE
    $<INSTALL_INTERFACE:pch.h>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/pch.h>
)
