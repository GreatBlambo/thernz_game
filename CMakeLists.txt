cmake_minimum_required(VERSION 3.2 FATAL_ERROR)
project(thernz_game VERSION 0.1 LANGUAGES CXX)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules" ${CMAKE_MODULE_PATH})

# Source files

set(SOURCES main.cpp
            rendering.cpp
            render_functions.cpp
            graphics.cpp
            error_codes.cpp)

add_executable(thernz_game ${SOURCES})

# Copy assets folder
file(GLOB_RECURSE ASSETS ${CMAKE_CURRENT_SOURCE_DIR}/assets/*)
add_custom_target(assets ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/assets assets
                         SOURCES ${ASSETS})

add_dependencies(thernz_game assets)

# Import libraries

find_package(OpenGL REQUIRED)
add_library(opengl INTERFACE IMPORTED)
set_property(TARGET opengl PROPERTY INTERFACE_LINK_LIBRARIES ${OPENGL_LIBRARY})

find_package(GLEW REQUIRED STATIC)
add_library(glew STATIC IMPORTED)
set_property(TARGET glew PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${GLEW_INCLUDE_DIRS})
set_property(TARGET glew PROPERTY IMPORTED_LOCATION ${GLEW_LIBRARY})

find_package(SDL2 REQUIRED STATIC)
add_library(sdl2 INTERFACE IMPORTED)
set_property(TARGET sdl2 PROPERTY INTERFACE_LINK_LIBRARIES ${SDL2_LIBRARY})
set_property(TARGET sdl2 PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${SDL2_INCLUDE_DIRS})

find_package(SDL2_image REQUIRED STATIC)
add_library(sdl2_image INTERFACE IMPORTED)
set_property(TARGET sdl2_image PROPERTY INTERFACE_LINK_LIBRARIES ${SDL2IMAGE_LIBRARY})

find_package(glm REQUIRED STATIC)

# Link target libraries

target_link_libraries(thernz_game
  opengl
  glew
  sdl2
  sdl2_image
  glm)
