#ifndef _T_TEXTURE_H_
#define _T_TEXTURE_H_

#include <stdint.h>

struct __attribute__((__packed__)) RGBColor {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
};
struct RGBAColor {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
    uint8_t alpha;
};
struct Texture {
    uint32_t width;
    uint32_t height;
    uint8_t* indices;
    struct RGBColor palette[256];
    struct RGBAColor* pixels;
};

struct Texture* image_load_pcx(const char* path);
void image_unload(struct Texture* img);

#endif