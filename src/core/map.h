#pragma once

#include <raylib.h>
#include <stdint.h>

typedef struct DFMap {
    uint16_t size;
    uint8_t* colormap;
    uint8_t* heightmap;
    Image image;
    Texture2D texture;
    Model model;
    Shader normalShd;
    Shader affineShd;
} DFMap;

DFMap* map_load();
void map_unload(DFMap* map);
void map_render(DFMap* map, bool wireframe);