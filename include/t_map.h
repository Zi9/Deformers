#pragma once

#include <raylib.h>
#include <stdint.h>

#define TEREP_MAPSZ 256
#define TEREP_TEXSZ 256
#define TEREP_HEIGHTMAP_FILE "map.pcx"
#define TEREP_COLORMAP_FILE "col.pcx"
#define TEREP_MAPTEX_FILE "maptex.pcx"

typedef struct TerepMap {
    uint16_t size;
    uint8_t* colormap;
    uint8_t* heightmap;
    Image image;
    Texture2D texture;
    Mesh mesh;
} TerepMap;

TerepMap* map_load();
void map_unload(TerepMap* map);
void map_render(TerepMap* map);