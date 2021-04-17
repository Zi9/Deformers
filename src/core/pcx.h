#pragma once

#include <raylib.h>
#include <stdint.h>

uint8_t* pcx_load_as_array(const char* path);
Image pcx_load_as_image(const char* path);