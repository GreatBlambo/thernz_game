set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

add_executable(sprite_batch_test sprite_batch_test/main.cpp)
target_link_libraries(sprite_batch_test ${LIBS})

add_executable(new_api_test new_api_test/main.cpp)
target_link_libraries(new_api_test ${LIBS})

add_executable(ds_test ds_test/main.cpp)
target_link_libraries(ds_test ${LIBS})
