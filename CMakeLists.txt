cmake_minimum_required(VERSION 3.0)
project(Deformers)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra")
include_directories(src)

file(GLOB_RECURSE SRC src/*.c)

add_custom_command(
    OUTPUT src/core/t_shaders.h
    COMMAND python ../tools/shaderbuild.py ../src/core/shaders/ ../src/core/
    DEPENDS ./src/core/shaders/affine.fs ./src/core/shaders/affine.vs
    VERBATIM
)

add_subdirectory(vendor/raylib)

add_executable(Deformers ${SRC} src/core/t_shaders.h)
target_link_libraries(Deformers raylib m)
