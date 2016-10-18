set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

add_executable(new_api_test new_api_test/main.cpp)
target_link_libraries(new_api_test ${LIBS})
