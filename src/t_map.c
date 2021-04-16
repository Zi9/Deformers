#include "t_map.h"

#include <math.h>
#include <rlgl.h>
#include <stdint.h>
#include <stdlib.h>

#include "t_pcx.h"
#include "t_shaders.h"

#define TMAP_SCALE 1.0f
#define TMAP_HEIGHT_SCALE 0.075f
#define TMAP_UVMULT 0.0625f

#define TEREP_MAPSZ 256
#define TEREP_HEIGHTMAP_FILE "map.pcx"
#define TEREP_COLORMAP_FILE "col.pcx"
#define TEREP_MAPTEX_FILE "maptex.pcx"

void build_map_model(TerepMap* map)
{
    Mesh msh = {0};
    msh.triangleCount = (map->size - 1) * (map->size - 1) * 2;
    msh.vertexCount = msh.triangleCount * 3;

    float* vertices = malloc((sizeof *vertices) * msh.vertexCount * 3);
    float* uvs = malloc((sizeof *uvs) * msh.vertexCount * 2);

    size_t vertC = 0;
    size_t uvC = 0;

    float uvx, uvy;

    for (uint16_t z = 0; z < map->size - 1; z++) {
        for (uint16_t x = 0; x < map->size - 1; x++) {
            vertices[vertC] = (x - map->size / 2.0f) * TMAP_SCALE;
            vertices[vertC + 1] = map->heightmap[x + z * map->size] * TMAP_HEIGHT_SCALE;
            vertices[vertC + 2] = (z - map->size / 2.0f) * TMAP_SCALE;

            vertices[vertC + 3] = (x - map->size / 2.0f) * TMAP_SCALE;
            vertices[vertC + 4] = map->heightmap[x + (z + 1) * map->size] * TMAP_HEIGHT_SCALE;
            vertices[vertC + 5] = (z - map->size / 2.0f + 1) * TMAP_SCALE;

            vertices[vertC + 6] = (x - map->size / 2.0f + 1) * TMAP_SCALE;
            vertices[vertC + 7] = map->heightmap[x + 1 + z * map->size] * TMAP_HEIGHT_SCALE;
            vertices[vertC + 8] = (z - map->size / 2.0f) * TMAP_SCALE;

            vertices[vertC + 9] = vertices[vertC + 6];
            vertices[vertC + 10] = vertices[vertC + 7];
            vertices[vertC + 11] = vertices[vertC + 8];

            vertices[vertC + 12] = vertices[vertC + 3];
            vertices[vertC + 13] = vertices[vertC + 4];
            vertices[vertC + 14] = vertices[vertC + 5];

            vertices[vertC + 15] = (x - map->size / 2.0f + 1) * TMAP_SCALE;
            vertices[vertC + 16] = map->heightmap[x + 1 + (z + 1) * map->size] * TMAP_HEIGHT_SCALE;
            vertices[vertC + 17] = (z - map->size / 2.0f + 1) * TMAP_SCALE;

            vertC += 18;

            uvy = (float)floor(map->colormap[x + z * TEREP_TEXSZ] / 16.0f) * TMAP_UVMULT;
            uvx = map->colormap[x + z * TEREP_TEXSZ] * TMAP_UVMULT;

            uvs[uvC] = uvx;
            uvs[uvC + 1] = uvy;

            uvs[uvC + 2] = uvx;
            uvs[uvC + 3] = uvy + TMAP_UVMULT;

            uvs[uvC + 4] = uvx + TMAP_UVMULT;
            uvs[uvC + 5] = uvy;

            uvs[uvC + 6] = uvs[uvC + 4];
            uvs[uvC + 7] = uvs[uvC + 5];

            uvs[uvC + 8] = uvs[uvC + 2];
            uvs[uvC + 9] = uvs[uvC + 3];

            uvs[uvC + 10] = uvx + TMAP_UVMULT;
            uvs[uvC + 11] = uvy + TMAP_UVMULT;

            uvC += 12;
        }
    }

    msh.vboId = malloc(sizeof(uint32_t) * 7);
    msh.vertices = vertices;
    msh.texcoords = uvs;
    rlLoadMesh(&msh, false);
    map->model = LoadModelFromMesh(msh);
}

TerepMap* map_load()
{
    TerepMap* map = malloc(sizeof *map);
    map->size = TEREP_MAPSZ;

    struct PCXData* pcx;

    pcx = pcx_load(TEREP_COLORMAP_FILE);
    map->colormap = pcx->indices;
    pcx_cleanup(pcx, false);

    pcx = pcx_load(TEREP_HEIGHTMAP_FILE);
    map->heightmap = pcx->indices;
    pcx_cleanup(pcx, false);

    pcx = pcx_load(TEREP_MAPTEX_FILE);
    pcx_to_image(pcx, &(map->image));
    pcx_cleanup(pcx, true);

    map->texture = LoadTextureFromImage(map->image);

    build_map_model(map);
    map->model.materials[0].shader = LoadShaderCode(affine_vs, affine_fs);
    map->model.materials[0].maps[MAP_DIFFUSE].texture = map->texture;
    return map;
}
void map_unload(TerepMap* map)
{
    free(map->colormap);
    free(map->heightmap);
    UnloadShader(map->model.materials[0].shader);
    UnloadModel(map->model);
    UnloadImage(map->image);
    UnloadTexture(map->texture);
    free(map);
}
void map_render(TerepMap* map, bool wireframe)
{
    if (wireframe)
        DrawModelWires(map->model, (Vector3){0.0f, -10.0f, 0.0f}, 1.0f, WHITE);
    else
        DrawModel(map->model, (Vector3){0.0f, -10.0f, 0.0f}, 1.0f, WHITE);
}