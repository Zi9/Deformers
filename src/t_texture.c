#include "t_texture.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

inline void texture_opengl_init(struct Texture* tex)
{
    glGenTextures(1, &tex->id);
    glBindTexture(GL_TEXTURE_2D, tex->id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, tex->width, tex->height, 0, GL_RGBA8, GL_UNSIGNED_BYTE, tex->pixels);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
}
void texture_generate_pixels(struct Texture* tex)
{
    tex->pixels = malloc(tex->width * tex->height * sizeof *tex->pixels);
    for (int i = 0; i < tex->width * tex->height; i++) {
        tex->pixels[i].red = tex->palette[tex->indices[i]].red;
        tex->pixels[i].green = tex->palette[tex->indices[i]].green;
        tex->pixels[i].blue = tex->palette[tex->indices[i]].blue;
        if (tex->indices[i] == 255) {
            tex->pixels[i].alpha = 255;
        } else {
            tex->pixels[i].alpha = 0;
        }
    }
}

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

struct Texture* texture_load_pcx(const char* path)
{
    struct Texture* tex = malloc(sizeof *tex);
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

    tex->width = TEREP_TEXSZ;
    tex->height = TEREP_TEXSZ;

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
    free(buf);
    free(hdr);

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
    tex->indices = indexbuf;

    uint8_t palmagic;
    fread(&palmagic, sizeof(palmagic), 1, fp);
    assert(palmagic == 12);
    assert(fread(&tex->palette, PCX_PALETTE_SIZE, 1, fp) == 1);

    fclose(fp);

    texture_generate_pixels(tex);
    texture_opengl_init(tex);

    return tex;
}
void texture_unload(struct Texture* tex)
{
    glDeleteTextures(1, &tex->id);
    free(tex->indices);
    free(tex->pixels);
    free(tex);
}