add_executable(sprite_batch_test main.cpp)
target_link_libraries(sprite_batch_test
                      game_lib
                      opengl
                      glew
                      sdl2
                      sdl2_image
                      sdl2_main)
