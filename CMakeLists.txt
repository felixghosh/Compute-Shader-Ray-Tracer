cmake_minimum_required(VERSION 2.8)
project(ComputeShaderRaytracer)

add_executable(raytracer src/main.c src/gl_utils.c src/linalg.c src/scene_buffer.c)
find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
find_package(glfw3 REQUIRED)
include_directories( ${OPENGL_INCLUDE_DIR})

set(CMAKE_CXX_FLAGS "-O3")
target_link_libraries(raytracer ${OPENGL_LIBRARIES} glfw glut GLEW m)
