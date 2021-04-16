#pragma once

#include <raylib.h>
#include <stdint.h>

#define TEREP_TEXSZ 256

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

struct PCXData* pcx_load(const char* path);
void pcx_to_image(struct PCXData* pcx, Image* img);
void pcx_cleanup(struct PCXData* pcx, bool freeIndex);