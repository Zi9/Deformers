#include "t_pcx.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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