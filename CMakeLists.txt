cmake_minimum_required(VERSION 3.2 FATAL_ERROR)
project(thernz_game VERSION 0.1 LANGUAGES CXX)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules" ${CMAKE_MODULE_PATH})

# Source files

set(SOURCES main.cpp)

add_executable(thernz_game ${SOURCES})

# Import libraries

find_package(OpenGL REQUIRED)
add_library(opengl INTERFACE IMPORTED)
set_property(TARGET opengl PROPERTY INTERFACE_LINK_LIBRARIES ${OPENGL_LIBRARY})

find_package(GLEW REQUIRED STATIC)
add_library(glew STATIC IMPORTED)
set_property(TARGET glew PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${GLEW_INCLUDE_DIRS})
set_property(TARGET glew PROPERTY IMPORTED_LOCATION ${GLEW_LIBRARY})

add_library(sdl2 INTERFACE IMPORTED)
find_package(SDL2 REQUIRED STATIC)
set_property(TARGET sdl2 PROPERTY INTERFACE_LINK_LIBRARIES ${SDL2_LIBRARY})

set_property(TARGET sdl2 PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${SDL2_INCLUDE_DIRS})

target_link_libraries(thernz_game
  opengl
  glew
  sdl2)
