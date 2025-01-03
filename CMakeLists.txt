cmake_minimum_required(VERSION 3.10)
project(VoxelEngine)

set(CMAKE_CXX_STANDARD 11)

# Export compile commands for SonarQube
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(OpenGL REQUIRED)

# Find GLFW
find_package(PkgConfig REQUIRED)
pkg_check_modules(GLFW REQUIRED glfw3)

include_directories(/usr/include)
link_directories(/usr/lib)

# Add source files
set(SOURCES
    external/glad/src/glad.c
    src/main.cpp
    src/Camera.cpp
    src/Input.cpp
    src/World.cpp
)

# Add executable
add_executable(voxel-engine ${SOURCES})

target_include_directories(voxel-engine PRIVATE external/glad/include)

# Link OpenGL library
find_package(OpenGL REQUIRED)
target_link_libraries(voxel-engine PRIVATE glfw OpenGL::GL)
