cmake_minimum_required(VERSION 3.2 FATAL_ERROR)
project(thernz_game VERSION 0.1 LANGUAGES CXX)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules" ${CMAKE_MODULE_PATH})

# Platform detection
if (WINDOWS)
   add_definitions(-DWINDOWS)
elseif(UNIX)
   add_definitions(-DUNIX)
endif()

# Source files

set(SOURCES rendering.cpp
            render_functions.cpp
	    render_types.cpp
            sprite_batch.cpp
            graphics.cpp
            error_codes.cpp
            game.cpp
            config.cpp
            mesh.cpp
            handle.cpp)

add_library(game_lib STATIC ${SOURCES})

include_directories("./")
add_executable(thernz_game main.cpp)
target_compile_features(thernz_game PRIVATE cxx_range_for)

add_definitions(-DGLEW_STATIC)

# Copy assets folder
file(GLOB_RECURSE ASSETS ${CMAKE_CURRENT_SOURCE_DIR}/assets/*)
add_custom_target(assets ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/assets assets
                         SOURCES ${ASSETS})

add_dependencies(game_lib assets)

# Third party

add_subdirectory(thirdparty)

# Import libraries

find_package(OpenGL REQUIRED)
add_library(opengl INTERFACE IMPORTED)
set_property(TARGET opengl PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${OPENGL_INCLUDE_DIRS})
set_property(TARGET opengl PROPERTY INTERFACE_LINK_LIBRARIES ${OPENGL_LIBRARY})

find_package(GLEW REQUIRED STATIC)
add_library(glew STATIC IMPORTED)
set_property(TARGET glew PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${GLEW_INCLUDE_DIRS})
set_property(TARGET glew PROPERTY IMPORTED_LOCATION ${GLEW_LIBRARY})

find_package(SDL2 REQUIRED)
add_library(sdl2 INTERFACE IMPORTED)
set_property(TARGET sdl2 PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${SDL2_INCLUDE_DIR})
set_property(TARGET sdl2 PROPERTY INTERFACE_LINK_LIBRARIES ${SDL2_LIBRARY})

add_library(sdl2_main INTERFACE IMPORTED)
set_property(TARGET sdl2_main PROPERTY INTERFACE_LINK_LIBRARIES ${SDL2_LIBRARY})

find_package(SDL2_image REQUIRED)
add_library(sdl2_image INTERFACE IMPORTED)
set_property(TARGET sdl2 PROPERTY INTERFACE_LINK_LIBRARIES ${SDL2IMAGE_LIBRARY})
set_property(TARGET sdl2_image PROPERTY INTERFACE_LINK_LIBRARIES ${SDL2IMAGE_LIBRARY})

find_package(glm REQUIRED STATIC)
include_directories(${GLM_INCLUDE_DIR})

# Link target libraries
set(LIBS game_lib
         opengl
         glew
         sdl2
         sdl2_image
         sdl2_main
         header_only_libs
         tinyobjloader)

add_subdirectory(tests)

target_link_libraries(thernz_game ${LIBS})
