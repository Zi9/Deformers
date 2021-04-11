#define WINDOW_TITLE "The Deformers"
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

#define TEREP_HEIGHTMAP_FILE "map.pcx"
#define TEREP_COLORMAP_FILE "col.pcx"
#define TEREP_MAPTEX_FILE "maptex.pcx"
#define TEREP_MAPSZ 256
#define TEREP_TEXSZ 256

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
struct Image {
    uint32_t width;
    uint32_t height;
    uint8_t* indices;
    struct RGBColor palette[256];
    struct RGBAColor* pixels;
};

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

struct Image* image_load_pcx(const char* path)
{
    // !NOTE: Reads default terep images as 256x256
    struct Image* img = malloc(sizeof(struct Image));
    FILE* fp = fopen(path, "r");
    if (fp == NULL) {
        printf("Unable to open PCX image %s for reading\n", path);
        return NULL;
    }
    struct PCXHeader* hdr = (struct PCXHeader*)malloc(PCX_HEADER_SIZE);
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
    // !NOTE: indexbuf will contain resized pixel index array
    // *This implementation is kinda hackish

    uint8_t* indexbuf = malloc(TEREP_TEXSZ * TEREP_TEXSZ * sizeof(uint8_t));
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

    img->pixels = malloc(TEREP_TEXSZ * TEREP_TEXSZ * sizeof(struct RGBAColor));
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
void image_unload(struct Image* img)
{
    free(img->indices);
    free(img->pixels);
    free(img);
}

struct TerepMap {
    // *NOTE: Add mesh, material and shader stuff in here too m8
    uint16_t xSize;
    uint16_t ySize;
    struct Image* heightmap;
    struct Image* colormap;
    struct Image* texture;
};
struct TerepMap* map_load()
{
    struct TerepMap* map = malloc(sizeof(struct TerepMap));
    map->colormap = image_load_pcx(TEREP_COLORMAP_FILE);
    map->heightmap = image_load_pcx(TEREP_HEIGHTMAP_FILE);
    map->texture = image_load_pcx(TEREP_MAPTEX_FILE);
    map->xSize = TEREP_MAPSZ;
    map->ySize = TEREP_MAPSZ;
    return map;
}
void map_unload(struct TerepMap* map)
{
    image_unload(map->colormap);
    image_unload(map->heightmap);
    image_unload(map->texture);
    free(map);
}
void map_render(struct TerepMap* map)
{
    // TODO: IMPLEMENT ME
}
// ----------------------------------------------------------------------------

GLFWwindow* window;
struct TerepMap* current_map;

void onGameStart() { current_map = map_load(); }
void onGameStop() { map_unload(current_map); }
void onInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_CAPS_LOCK) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}
void onUpdate() {}
void onRender()
{
    glRasterPos2f(-1, 1);
    glPixelZoom(1, -1);
    glDrawPixels(current_map->colormap->width, current_map->colormap->height, GL_RGBA, GL_UNSIGNED_BYTE,
                 current_map->colormap->pixels);
}

int main(int argc, char** argv)
{
    if (!glfwInit())
        return -1;
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE, NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    onGameStart();
    while (!glfwWindowShouldClose(window)) {
        onInput(window);

        onUpdate();

        glClear(GL_COLOR_BUFFER_BIT);
        onRender();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    onGameStop();
    glfwTerminate();
    return 0;
}