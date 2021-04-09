#define WINDOW_TITLE "The Deformers"
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdbool.h>
#include <stdio.h>

GLFWwindow* window;
int win_width, win_height;



char PCX_defaultPalette[48] = {
	0x00, 0x00, 0x00,    0x00, 0x00, 0x80,    0x00, 0x80, 0x00,
	0x00, 0x80, 0x80,    0x80, 0x00, 0x00,    0x80, 0x00, 0x80,
	0x80, 0x80, 0x00,    0x80, 0x80, 0x80,    0xc0, 0xc0, 0xc0,
	0x00, 0x00, 0xff,    0x00, 0xff, 0x00,    0x00, 0xff, 0xff,
	0xff, 0x00, 0x00,    0xff, 0x00, 0xff,    0xff, 0xff, 0x00,
	0xff, 0xff, 0xff 
};

struct PCXFile {
    uint8_t identifier;
    uint8_t version;
    uint8_t encoding;
    uint8_t bitsPerPixel;

    uint16_t xMin;
    uint16_t yMin;
    uint16_t xMax;
    uint16_t yMax;

    int width;
    int height;

    uint16_t hDPI;
    uint16_t vDPI;

    uint8_t colmap[48];

    uint8_t reserved;
    uint8_t nplanes;
    uint16_t bytesPerLine;
    uint16_t paletteInfo;
    uint16_t hScreenSize;
    uint16_t vScreenSize;
};

void PCX_load_header(FILE* fp, struct PCXFile* pcx)
{
    fread(&(pcx->identifier), sizeof(pcx->identifier), 1, fp);
    fread(&(pcx->version), sizeof(pcx->version), 1, fp);
    fread(&(pcx->encoding), sizeof(pcx->encoding), 1, fp);
    fread(&(pcx->bitsPerPixel), sizeof(pcx->bitsPerPixel), 1, fp);

    fread(&(pcx->xMin), sizeof(pcx->xMin), 1, fp);
    fread(&(pcx->yMin), sizeof(pcx->yMin), 1, fp);
    fread(&(pcx->xMax), sizeof(pcx->xMax), 1, fp);
    fread(&(pcx->yMax), sizeof(pcx->yMax), 1, fp);

    pcx->width = pcx->xMax - pcx->xMin + 1;
    pcx->height = pcx->yMax - pcx->yMin + 1;

    fread(&(pcx->hDPI), sizeof(pcx->hDPI), 1, fp);
    fread(&(pcx->vDPI), sizeof(pcx->vDPI), 1, fp);

    for (int i=0; i<48; i++)
    {
        fread(&(pcx->colmap[i]), sizeof(pcx->colmap[0]), 1, fp);
    }
    
    fread(&(pcx->reserved), sizeof(pcx->reserved), 1, fp);
    fread(&(pcx->nplanes), sizeof(pcx->nplanes), 1, fp);
    fread(&(pcx->bytesPerLine), sizeof(pcx->bytesPerLine), 1, fp);
    fread(&(pcx->paletteInfo), sizeof(pcx->paletteInfo), 1, fp);
    fread(&(pcx->hScreenSize), sizeof(pcx->hScreenSize), 1, fp);
    fread(&(pcx->vScreenSize), sizeof(pcx->vScreenSize), 1, fp);
}

void PCX_load_image(const char* path)
{
    FILE* fp = fopen(path, "r");
    if (fp == NULL)
    {
        printf("Unable to open PCX image %s for reading\n", path);
        return;
    }
    struct PCXFile pcx = {0};
    PCX_load_header(fp, &pcx);


    printf("Manufac:%i Ver:%i Enc:%i Bpp:%i\n", pcx.identifier, pcx.version, pcx.encoding, pcx.bitsPerPixel);
    printf("Width:%i Height:%i\n", pcx.width, pcx.height);
    printf("DPI:%ix%i\n", pcx.hDPI, pcx.vDPI);
    printf("Reserved:%i NPlanes:%i BPL:%i PalInfo:%i\n", pcx.reserved, pcx.nplanes, pcx.bytesPerLine, pcx.paletteInfo);
    printf("ScreenSize:%ix%i\n", pcx.hScreenSize, pcx.vScreenSize);

    fclose(fp);
}




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
    PCX_load_image("test.pcx");
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

    glfwGetFramebufferSize(window, &win_width, &win_height);
    glViewport(0, 0, win_width, win_height);
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