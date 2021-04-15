#pragma once

#include <raylib.h>
#include <stdint.h>

typedef struct TerepMap {
    uint16_t size;
    uint8_t* colormap;
    uint8_t* heightmap;
    Image image;
    Texture2D texture;
    Model model;
} TerepMap;

TerepMap* map_load();
void map_unload(TerepMap* map);
void map_render(TerepMap* map);