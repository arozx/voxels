cmake_minimum_required(VERSION 3.10)
project(Voxels)

set(CMAKE_CXX_STANDARD 11)

# Export compile commands for SonarQube
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(GLEW REQUIRED)

# Add GLM manually
set(GLM_INCLUDE_DIRS "/usr/include/glm")
include_directories(${GLM_INCLUDE_DIRS})

add_executable(voxels src/main.cpp)

target_link_libraries(voxels ${OPENGL_LIBRARIES} glfw GLEW::GLEW)