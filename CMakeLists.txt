cmake_minimum_required(VERSION 3.0)
project(TheDeformers)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra")
include_directories(include)

file(GLOB SRC src/*.c)

add_custom_command(
    OUTPUT include/t_shaders.h
    COMMAND python ../shaderbuild.py ../shaders/ ../include/
    DEPENDS ./shaders/*
    VERBATIM
)

add_executable(TheDeformers ${SRC} include/t_shaders.h)
target_link_libraries(TheDeformers raylib m)
