cmake_minimum_required(VERSION 3.0)
project(TheDeformers)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")
include_directories(include)

file(GLOB SRC src/*.c)

add_executable(TheDeformers ${SRC})
target_link_libraries(TheDeformers raylib)