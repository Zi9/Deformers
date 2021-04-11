#define WINDOW_TITLE "The Deformers"
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

#define TEREP_HEIGHTMAP_FILE "map.pcx"
#define TEREP_COLORMAP_FILE "col.pcx"
#define TEREP_MAPTEX_FILE "maptex.pcx"
#define TEREP_MAPSZ 256

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

GLFWwindow* window;


// IMAGE STUFF
struct __attribute__((__packed__)) RGBColor {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
};
struct Image {
    uint32_t width;
    uint32_t height;
    uint8_t* indices;
    struct RGBColor palette[256];
    struct RGBColor* pixels;
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
    struct Image* img = malloc(sizeof(struct Image));
    FILE* fp = fopen(path, "r");
    if (fp == NULL)
    {
        printf("Unable to open PCX image %s for reading\n", path);
        return NULL;
    }
    struct PCXHeader* hdr = (struct PCXHeader*)malloc(PCX_HEADER_SIZE);
    assert(fread(hdr, PCX_HEADER_SIZE, 1, fp) == 1);
    if (hdr->identifier != 0x0A)
    {
        fclose(fp);
        free(hdr);
        printf("%s is not a valid pcx file\n", path);
        return NULL;
    }

    assert(hdr->encoding == 1);
    assert(hdr->bitsPerPixel == 8);
    assert(hdr->nplanes == 1);

    // printf("\nFile: %s\n", path);
    // printf("Ver:%i Enc:%i Bpp:%i DPI:%ix%i Reserved:%i NPlanes:%i BPL:%i PalInfo:%i\n",
    //        hdr->version, hdr->encoding, hdr->bitsPerPixel, hdr->hDPI, hdr->vDPI,
    //        hdr->reserved, hdr->nplanes, hdr->bytesPerLine, hdr->paletteInfo);

    img->width = hdr->xMax - hdr->xMin + 1;
    img->height = hdr->yMax - hdr->yMin + 1;
    uint32_t bufsz = hdr->bytesPerLine * hdr->nplanes * img->height;
    // printf("Bufsz = %i\n", bufsz);

    uint8_t* buf = malloc(bufsz);
    uint8_t in;
    uint8_t repe;
    for (uint32_t bufi = 0; bufi < bufsz;)
    {
        if (fread(&in, sizeof(in), 1, fp) == 0) break;
        if ((0xC0 & in) == 0xC0)
        {
            repe = 0x3F & in;
            assert(fread(&in, sizeof(in), 1, fp) != 0);
            memset(buf+bufi, in, repe);
            bufi += repe;
        }
        else
        {
            *(buf+bufi) = in;
            bufi++;
        }
    }
    img->indices = buf;

    uint8_t palmagic;
    fread(&palmagic, sizeof(palmagic), 1, fp);
    assert(palmagic == 12);
    assert(fread(&img->palette, PCX_PALETTE_SIZE, 1, fp) == 1);

    img->pixels = malloc(img->width*img->height*sizeof(struct RGBColor));
    for (int i = 0; i < img->width*img->height; i++)
    {
        img->pixels[i].red = img->palette[img->indices[i]].red;
        img->pixels[i].green = img->palette[img->indices[i]].green;
        img->pixels[i].blue = img->palette[img->indices[i]].blue;
    }

    fclose(fp);
    free(hdr);

    return img;
}
void image_unload(struct Image* img)
{
    free(img->indices);
    free(img->pixels);
    free(img);
}
// ----------------------------------------------------------------------------

// MAP STUFF
struct TerepMap {
    // *NOTE: Only square maps
    uint16_t size;
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
    map->size = TEREP_MAPSZ;
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


void onInput(GLFWwindow* window)
{
    if(glfwGetKey(window, GLFW_KEY_CAPS_LOCK) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}
void onUpdate()
{

}
void onRender()
{

}

int main(int argc, char** argv)
{
    struct TerepMap* a = map_load();
    map_unload(a);
    return 0;

    if (!glfwInit())
        return -1;
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE, NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    while (!glfwWindowShouldClose(window))
    {
        onInput(window);
        glClear(GL_COLOR_BUFFER_BIT);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}