cmake_minimum_required(VERSION 3.0.0)
project(offscreen VERSION 0.1.0)

set(LINK_DIR ${PROJECT_SOURCE_DIR}/lib64)
link_directories(${LINK_DIR})
link_libraries(glfw)

# FIND OPENGL
#########################################################
find_package(OpenGL REQUIRED)
include_directories(${OpenGL_INCLUDE_DIRS})
link_directories(${OpenGL_LIBRARY_DIRS})
add_definitions(${OpenGL_DEFINITIONS})
if(NOT OPENGL_FOUND)
    message(ERROR " OPENGL not found!")
endif(NOT OPENGL_FOUND)

add_executable(offscreen glad.c toojpeg.cpp main.cpp)

target_link_libraries(offscreen glfw ${OPENGL_LIBRARIES} ${CMAKE_DL_LIBS} )
