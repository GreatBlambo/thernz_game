set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

add_executable(sprite_batch_test sprite_batch_test/main.cpp)
target_link_libraries(sprite_batch_test ${LIBS})
