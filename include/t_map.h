#ifndef _T_MAP_H_
#define _T_MAP_H_

#include <GL/glew.h>
#include <stdint.h>

#include "t_texture.h"

#define TEREP_MAPSZ 256
#define TEREP_HEIGHTMAP_FILE "map.pcx"
#define TEREP_COLORMAP_FILE "col.pcx"
#define TEREP_MAPTEX_FILE "maptex.pcx"


struct Map {
    // *NOTE: Add mesh, material and shader stuff in here too m8
    uint16_t xSize;
    uint16_t ySize;
    struct Texture* heightmap;
    struct Texture* colormap;
    struct Texture* texture;

    GLuint vao;
    GLuint vbo;
    GLuint shader;
    GLuint glTex;
};

struct Map* map_load();
void map_unload(struct Map* map);
void map_render(struct Map* map);

#endif