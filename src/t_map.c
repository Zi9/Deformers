#include "t_map.h"

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <rlgl.h>

#define TMAP_SCALE 1.0f
#define TMAP_HEIGHT_SCALE 0.075f
#define TMAP_UVMULT 0.0625f

#define TEREP_MAPSZ 256
#define TEREP_TEXSZ 256
#define TEREP_HEIGHTMAP_FILE "map.pcx"
#define TEREP_COLORMAP_FILE "col.pcx"
#define TEREP_MAPTEX_FILE "maptex.pcx"

#define PCX_HEADER_SIZE 128
#define PCX_PALETTE_SIZE 768
struct __attribute__((__packed__)) RGBColor {
    uint8_t red, green, blue;
};
struct __attribute__((__packed__)) PCXHeader {
    uint8_t identifier;
    uint8_t version;
    uint8_t encoding;
    uint8_t bitsPerPixel;

    uint16_t xMin;
    uint16_t yMin;
    uint16_t xMax;
    uint16_t yMax;

    uint16_t hDPI;
    uint16_t vDPI;

    struct RGBColor colmap[16];

    uint8_t reserved;
    uint8_t nplanes;
    uint16_t bytesPerLine;
    uint16_t paletteInfo;
};
struct PCXData {
    uint8_t* indices;
    struct RGBColor palette[256];
};

struct PCXData* pcx_load(const char* path)
{
    struct PCXData* pcx = malloc(sizeof *pcx);
    FILE* fp = fopen(path, "r");
    if (fp == NULL) {
        printf("Unable to open PCX image %s for reading\n", path);
        return NULL;
    }
    struct PCXHeader* hdr = malloc(PCX_HEADER_SIZE);
    assert(fread(hdr, PCX_HEADER_SIZE, 1, fp) == 1);
    if (hdr->identifier != 0x0A) {
        fclose(fp);
        free(hdr);
        printf("%s is not a valid pcx file\n", path);
        return NULL;
    }

    assert(hdr->encoding == 1);
    assert(hdr->bitsPerPixel == 8);
    assert(hdr->nplanes == 1);

    uint16_t width = hdr->xMax - hdr->xMin + 1;
    uint16_t height = hdr->yMax - hdr->yMin + 1;
    assert(width >= 256);

    size_t bufsz = hdr->bytesPerLine * hdr->nplanes * height;
    uint8_t* buf = malloc(bufsz);
    uint8_t in, repe;
    for (size_t bufi = 0; bufi < bufsz;) {
        if (fread(&in, sizeof(in), 1, fp) == 0)
            break;
        if ((0xC0 & in) == 0xC0) {
            repe = 0x3F & in;
            assert(fread(&in, sizeof(in), 1, fp) != 0);
            memset(buf + bufi, in, repe);
            bufi += repe;
        } else {
            *(buf + bufi) = in;
            bufi++;
        }
    }
    free(hdr);

    pcx->indices = malloc(TEREP_TEXSZ * TEREP_TEXSZ * sizeof *pcx->indices);
    if (height >= TEREP_TEXSZ) {
        for (size_t y = 0; y < TEREP_TEXSZ; y++) {
            memcpy(pcx->indices + (y * TEREP_TEXSZ), buf + (y * width), TEREP_TEXSZ * sizeof(uint8_t));
        }
    } else {
        size_t wrote_pixels = 0;
        for (size_t y = 0; y < height; y++) {
            memcpy(pcx->indices + (y * TEREP_TEXSZ), buf + (y * width), TEREP_TEXSZ * sizeof(uint8_t));
            wrote_pixels += TEREP_TEXSZ * sizeof(uint8_t);
        }
        memset(pcx->indices + wrote_pixels, 0xFF, (TEREP_TEXSZ * TEREP_TEXSZ * sizeof(uint8_t)) - wrote_pixels);
    }
    free(buf);

    uint8_t palmagic;
    fread(&palmagic, sizeof(palmagic), 1, fp);
    assert(palmagic == 12);
    assert(fread(&pcx->palette, PCX_PALETTE_SIZE, 1, fp) == 1);

    fclose(fp);

    return pcx;
}
void pcx_to_image(struct PCXData* pcx, Image* img)
{
    Color* pix = malloc(TEREP_TEXSZ * TEREP_TEXSZ * sizeof *pix);
    for (size_t i = 0; i < TEREP_TEXSZ * TEREP_TEXSZ; i++) {
        pix[i].r = pcx->palette[pcx->indices[i]].red;
        pix[i].g = pcx->palette[pcx->indices[i]].green;
        pix[i].b = pcx->palette[pcx->indices[i]].blue;
        if (pcx->indices[i] == 255) {
            pix[i].a = 0;
        } else {
            pix[i].a = 255;
        }
    }
    img->data = pix;
    img->width = TEREP_TEXSZ;
    img->height = TEREP_TEXSZ;
    img->format = 7;
    img->mipmaps = 1;
}
void pcx_cleanup(struct PCXData* pcx, bool freeIndex)
{
    if (freeIndex)
        free(pcx->indices);
    free(pcx);
}

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
            vertices[vertC]    = (x - map->size/2.0f) * TMAP_SCALE;
            vertices[vertC+1]  = map->heightmap[x + z * map->size] * TMAP_HEIGHT_SCALE;
            vertices[vertC+2]  = (z - map->size/2.0f) * TMAP_SCALE;

            vertices[vertC+3]  = (x - map->size/2.0f) * TMAP_SCALE;
            vertices[vertC+4]  = map->heightmap[x + (z + 1) * map->size] * TMAP_HEIGHT_SCALE;
            vertices[vertC+5]  = (z - map->size/2.0f + 1) * TMAP_SCALE;

            vertices[vertC+6]  = (x - map->size/2.0f + 1) * TMAP_SCALE;
            vertices[vertC+7]  = map->heightmap[x + 1 + z * map->size] * TMAP_HEIGHT_SCALE;
            vertices[vertC+8]  = (z - map->size/2.0f) * TMAP_SCALE;

            vertices[vertC+9]  = vertices[vertC+6];
            vertices[vertC+10] = vertices[vertC+7];
            vertices[vertC+11] = vertices[vertC+8];

            vertices[vertC+12] = vertices[vertC+3];
            vertices[vertC+13] = vertices[vertC+4];
            vertices[vertC+14] = vertices[vertC+5];

            vertices[vertC+15] = (x - map->size/2.0f + 1) * TMAP_SCALE;
            vertices[vertC+16] = map->heightmap[x + 1 + (z + 1) * map->size] * TMAP_HEIGHT_SCALE;
            vertices[vertC+17] = (z - map->size/2.0f + 1) * TMAP_SCALE;

            vertC += 18;

            uvy = (float)floor(map->colormap[x + z * TEREP_TEXSZ] / 16.0f) * TMAP_UVMULT;
            uvx = map->colormap[x + z * TEREP_TEXSZ] * TMAP_UVMULT;

            uvs[uvC]   = uvx;
            uvs[uvC+1] = uvy;

            uvs[uvC+2] = uvx;
            uvs[uvC+3] = uvy + TMAP_UVMULT;

            uvs[uvC+4] = uvx + TMAP_UVMULT;
            uvs[uvC+5] = uvy;

            uvs[uvC+6] = uvs[uvC+4];
            uvs[uvC+7] = uvs[uvC+5];

            uvs[uvC+8] = uvs[uvC+2];
            uvs[uvC+9] = uvs[uvC+3];

            uvs[uvC+10] = uvx + TMAP_UVMULT;
            uvs[uvC+11] = uvy + TMAP_UVMULT;

            uvC += 12;
        }
    }

    msh.vboId = malloc(sizeof(uint32_t) * 7);
    msh.vertices = vertices;
    msh.texcoords = uvs;
    rlLoadMesh(&msh, false);
    map->model = LoadModelFromMesh(msh);
}

const char* vs = "#version 330\n"
                 "in vec4 vertexPosition;"
                 "in vec2 vertexTexCoord;"
                 "uniform mat4 mvp;"
                 "out vec3 fragTexCoord;"
                 "void main() {"
                    "gl_Position = mvp*vec4(vertexPosition);"
                    "fragTexCoord = vec3(vertexTexCoord*gl_Position.z, gl_Position.z);"
                 "}";

const char* fs = "#version 330\n"
                 "in vec3 fragTexCoord;"
                 "uniform sampler2D texture0;"
                 "out vec4 finalColor;"
                 "void main() {"
                    "finalColor = texture(texture0, fragTexCoord.xy/fragTexCoord.z);"
                 "}";

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
    map->model.materials[0].shader = LoadShaderCode(vs, fs);
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