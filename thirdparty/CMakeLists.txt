include_directories(tinyobjloader)

add_subdirectory(tinyobjloader)
add_subdirectory(bitsquid-foundation-git)

add_library(header_only_libs STATIC header_only_impls.cpp)
