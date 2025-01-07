cmake_minimum_required(VERSION 3.10)
project(VoxelEngine)

set(CMAKE_CXX_STANDARD 11)

# Export compile commands for SonarQube
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(OpenGL_GL_PREFERENCE GLVND)
find_package(OpenGL REQUIRED)

# Find GLFW
find_package(PkgConfig REQUIRED)
pkg_check_modules(GLFW REQUIRED glfw3)

# Find FreeType
find_package(Freetype REQUIRED)

include_directories(/usr/include)
include_directories(${FREETYPE_INCLUDE_DIRS})
link_directories(/usr/lib)

# Copy the INI file to the build directory
file(COPY ${CMAKE_SOURCE_DIR}/conf.ini DESTINATION ${CMAKE_BINARY_DIR})

# Add source files
set(SOURCES
    external/glad/src/glad.c
    src/main.cpp
    src/Camera.cpp
    src/Input.cpp
    src/World.cpp
    src/PerlinNoise.cpp
    src/Octree.cpp
    src/Frustum.cpp
    src/TextRenderer.cpp
    src/Renderer.cpp
    src/LightingManager.cpp
    src/ConfigLoader.cpp
    src/Chunk.cpp
)

# Add executable
add_executable(voxel-engine ${SOURCES})

target_include_directories(voxel-engine PRIVATE external/glad/include)

# Link OpenGL and FreeType libraries
find_package(OpenGL REQUIRED)
target_link_libraries(voxel-engine PRIVATE glfw OpenGL::GL ${FREETYPE_LIBRARIES})

# Set Clang-Tidy as the CMake static analysis tool
set(CMAKE_CXX_CLANG_TIDY "clang-tidy;-checks=*,-modernize-use-auto")
