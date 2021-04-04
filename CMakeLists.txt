cmake_minimum_required(VERSION 3.0)
project(TheDeformers)

add_executable(TheDeformers the-deformers.c)
target_link_libraries(TheDeformers glfw GL)