#include "t_map.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <rlgl.h>

#define PCX_HEADER_SIZE 128
#define PCX_PALETTE_SIZE 768
struct __attribute__((__packed__)) RGBColor {
    unsigned char red, green, blue;
};
struct __attribute__((__packed__)) PCXHeader {
    unsigned char identifier;
    unsigned char version;
    unsigned char encoding;
    unsigned char bitsPerPixel;

    unsigned short xMin;
    unsigned short yMin;
    unsigned short xMax;
    unsigned short yMax;

    unsigned short hDPI;
    unsigned short vDPI;

    struct RGBColor colmap[16];

    unsigned char reserved;
    unsigned char nplanes;
    unsigned short bytesPerLine;
    unsigned short paletteInfo;
};
struct PCXData {
    unsigned char* indices;
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

    unsigned short width = hdr->xMax - hdr->xMin + 1;
    unsigned short height = hdr->yMax - hdr->yMin + 1;
    assert(width >= 256);

    unsigned int bufsz = hdr->bytesPerLine * hdr->nplanes * height;
    unsigned char* buf = malloc(bufsz);
    unsigned char in, repe;
    for (unsigned int bufi = 0; bufi < bufsz;) {
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
    free(buf);
    free(hdr);

    pcx->indices = malloc(TEREP_TEXSZ * TEREP_TEXSZ * sizeof *pcx->indices);
    if (height >= TEREP_TEXSZ) {
        for (size_t y = 0; y < TEREP_TEXSZ; y++) {
            memcpy(pcx->indices + (y * TEREP_TEXSZ), buf + (y * width), TEREP_TEXSZ * sizeof(unsigned char));
        }
    } else {
        unsigned int wrote_pixels = 0;
        for (size_t y = 0; y < height; y++) {
            memcpy(pcx->indices + (y * TEREP_TEXSZ), buf + (y * width), TEREP_TEXSZ * sizeof(unsigned char));
            wrote_pixels += TEREP_TEXSZ * sizeof(unsigned char);
        }
        memset(pcx->indices + wrote_pixels, 0xFF, (TEREP_TEXSZ * TEREP_TEXSZ * sizeof(unsigned char)) - wrote_pixels);
    }

    unsigned char palmagic;
    fread(&palmagic, sizeof(palmagic), 1, fp);
    assert(palmagic == 12);
    assert(fread(&pcx->palette, PCX_PALETTE_SIZE, 1, fp) == 1);

    fclose(fp);

    return pcx;
}

void pcx_to_image(struct PCXData* pcx, Image* img)
{
    Color* pix = malloc(TEREP_TEXSZ * TEREP_TEXSZ * sizeof *pix);
    for (int i = 0; i < TEREP_TEXSZ * TEREP_TEXSZ; i++) {
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

TerepMap* map_load()
{
    TerepMap* map = malloc(sizeof *map);
    map->size = TEREP_MAPSZ;

    struct PCXData* pcx;
    pcx = pcx_load(TEREP_COLORMAP_FILE);
    map->colormap = pcx->indices;
    free(pcx);
    pcx = pcx_load(TEREP_HEIGHTMAP_FILE);
    map->heightmap = pcx->indices;
    free(pcx);
    pcx = pcx_load(TEREP_MAPTEX_FILE);
    pcx_to_image(pcx, &(map->image));
    free(pcx);
    map->texture = LoadTextureFromImage(map->image);
    return map;
}
void map_unload(TerepMap* map)
{
    free(map->colormap);
    free(map->heightmap);
    UnloadImage(map->image);
    UnloadTexture(map->texture);
    free(map);
}
void map_render(TerepMap* map)
{
    DrawTexture(map->texture, 0, 0, WHITE);
}