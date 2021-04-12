#include "texture.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define TEREP_TEXSZ 256

#define PCX_HEADER_SIZE 128
#define PCX_PALETTE_SIZE 768
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

struct Texture* image_load_pcx(const char* path)
{
    struct Texture* img = malloc(sizeof *img);
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

    uint32_t width = hdr->xMax - hdr->xMin + 1;
    uint32_t height = hdr->yMax - hdr->yMin + 1;
    assert(width >= 256);
    uint32_t bufsz = hdr->bytesPerLine * hdr->nplanes * height;
    uint8_t* buf = malloc(bufsz);
    uint8_t in;
    uint8_t repe;
    for (uint32_t bufi = 0; bufi < bufsz;) {
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

    uint8_t* indexbuf = malloc(TEREP_TEXSZ * TEREP_TEXSZ * sizeof *indexbuf);
    if (height >= TEREP_TEXSZ) {
        for (int y = 0; y < TEREP_TEXSZ; y++) {
            memcpy(indexbuf + (y * TEREP_TEXSZ), buf + (y * width), TEREP_TEXSZ * sizeof(uint8_t));
        }
    } else {
        uint32_t wrote_pixels = 0;
        for (int y = 0; y < height; y++) {
            memcpy(indexbuf + (y * TEREP_TEXSZ), buf + (y * width), TEREP_TEXSZ * sizeof(uint8_t));
            wrote_pixels += TEREP_TEXSZ * sizeof(uint8_t);
        }
        memset(indexbuf + wrote_pixels, 0xFF, (TEREP_TEXSZ * TEREP_TEXSZ * sizeof(uint8_t)) - wrote_pixels);
    }
    img->indices = indexbuf;

    uint8_t palmagic;
    fread(&palmagic, sizeof(palmagic), 1, fp);
    assert(palmagic == 12);
    assert(fread(&img->palette, PCX_PALETTE_SIZE, 1, fp) == 1);

    img->pixels = malloc(TEREP_TEXSZ * TEREP_TEXSZ * sizeof *img->pixels);
    for (int i = 0; i < TEREP_TEXSZ * TEREP_TEXSZ; i++) {
        img->pixels[i].red = img->palette[img->indices[i]].red;
        img->pixels[i].green = img->palette[img->indices[i]].green;
        img->pixels[i].blue = img->palette[img->indices[i]].blue;
        if (img->indices[i] == 255) {
            img->pixels[i].alpha = 255;
        } else {
            img->pixels[i].alpha = 0;
        }
    }

    free(buf);
    free(hdr);
    fclose(fp);

    img->width = TEREP_TEXSZ;
    img->height = TEREP_TEXSZ;

    return img;
}

void image_unload(struct Texture* img)
{
    free(img->indices);
    free(img->pixels);
    free(img);
}