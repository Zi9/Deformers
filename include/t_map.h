#pragma once

#include <raylib.h>

#define TEREP_MAPSZ 256
#define TEREP_TEXSZ 256
#define TEREP_HEIGHTMAP_FILE "map.pcx"
#define TEREP_COLORMAP_FILE "col.pcx"
#define TEREP_MAPTEX_FILE "maptex.pcx"


typedef struct TerepMap {
    unsigned short size;
    unsigned char* colormap;
    unsigned char* heightmap;
    Image image;
    Texture2D texture;
    Mesh mesh;
} TerepMap;

TerepMap* map_load();
void map_unload(TerepMap* map);
void map_render(TerepMap* map);