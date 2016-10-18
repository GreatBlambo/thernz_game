include_directories(tinyobjloader)

add_subdirectory(tinyobjloader)

# bitsquid foundation
add_subdirectory(bitsquid-foundation-git)
include_directories(bitsquid-foundation-git)

# header only library implementations

add_library(header_only_libs STATIC header_only_impls.cpp)
