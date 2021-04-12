#define WINDOW_TITLE "The Deformers"
#define WINDOW_WIDTH 960
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

// TODO: Add error checking to this
GLuint compile_shader(const char* vertexShader, const char* fragmentShader)
{
    GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragmetShaderID = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(vertexShaderID, 1, &vertexShader, NULL);
    glCompileShader(vertexShaderID);
    glShaderSource(fragmetShaderID, 1, &fragmentShader, NULL);
    glCompileShader(fragmetShaderID);
    GLuint ProgramID = glCreateProgram();
    glAttachShader(ProgramID, vertexShaderID);
    glAttachShader(ProgramID, fragmetShaderID);
    glLinkProgram(ProgramID);

    glDetachShader(ProgramID, vertexShaderID);
    glDetachShader(ProgramID, fragmetShaderID);

    glDeleteShader(vertexShaderID);
    glDeleteShader(fragmetShaderID);
    return ProgramID;
}




const char* mapVertexShader = "#version 330 core\n"
                              "layout (location = 0) in vec3 apos;\n"
                              "layout (location = 1) in vec2 UVin;\n"
                              "out vec2 UV;\n"
                              "uniform mat4 MVP;\n"
                              "void main() {\n"
                              "   gl_Position = MVP * vec4(apos, 1);\n"
                              "   UV = UVin;\n"
                              "}";

const char* mapFragmentShader = "#version 330 core\n"
                                "in vec2 UV;\n"
                                "out vec3 color;\n"
                                "uniform sampler2D texSamp;\n"
                                "void main() {\n"
                                "   color = texture(texSamp, UV).rgb;\n"
                                "}";

struct Map {
    // *NOTE: Add mesh, material and shader stuff in here too m8
    uint16_t xSize;
    uint16_t ySize;
    struct Image* heightmap;
    struct Image* colormap;
    struct Image* texture;

    GLuint vao;
    GLuint vbo;
    GLuint shader;
    GLuint glTex;
};
static const GLfloat tempdatabuf[] = {
    -0.8f,  0.8f, 0.0f,     0.0f, 1.0f,
    -0.8f, -0.8f, 0.0f,     1.0f, 1.0f,
     0.8f,  0.8f, 0.0f,     0.0f, 0.0f,

     0.8f,  0.8f, 0.0f,     1.0f, 1.0f,
    -0.8f, -0.8f, 0.0f,     0.0f, 0.0f,
     0.8f, -0.8f, 0.0f,     1.0f, 0.0f,
};
struct Map* map_load()
{
    struct Map* map = malloc(sizeof(struct Map));
    map->colormap = image_load_pcx(TEREP_COLORMAP_FILE);
    map->heightmap = image_load_pcx(TEREP_HEIGHTMAP_FILE);
    map->texture = image_load_pcx(TEREP_MAPTEX_FILE);
    map->xSize = TEREP_MAPSZ;
    map->ySize = TEREP_MAPSZ;

    glGenVertexArrays(1, &map->vao);
    glBindVertexArray(map->vao);
    glGenBuffers(1, &map->vbo);
    glBindBuffer(GL_ARRAY_BUFFER, map->vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(tempdatabuf), tempdatabuf, GL_STATIC_DRAW);
    map->shader = compile_shader(mapVertexShader, mapFragmentShader);

    // glGenTextures(1, &map->glTex);
    // glBindTexture(GL_TEXTURE_2D, map->glTex);
    // glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, map->texture->width, map->texture->height, 0, GL_RGBA8, GL_UNSIGNED_BYTE, map->texture->pixels);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    return map;
}
void map_unload(struct Map* map)
{
    // glDeleteTextures(1, &map->glTex);
    glDeleteProgram(map->shader);
    glDeleteBuffers(1, &map->vbo);
    glDeleteVertexArrays(1, &map->vao);

    image_unload(map->colormap);
    image_unload(map->heightmap);
    image_unload(map->texture);
    free(map);
}
void map_render(struct Map* map)
{
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, map->vbo);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glUseProgram(map->shader);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    glDisableVertexAttribArray(0);
}

GLFWwindow* window;
struct Map* current_map;


void onGameStart()
{
    current_map = map_load();
}
void onGameStop()
{
    map_unload(current_map);
}
void onInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_CAPS_LOCK) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}
void onUpdate() {}
void onRender()
{
    map_render(current_map);
}

int main(int argc, char** argv)
{
    if (!glfwInit())
        return -1;
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE, NULL, NULL);
    if (!window) {
        printf("Could not create window\n");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glewExperimental = true;
    if (glewInit() != GLEW_OK) {
        printf("Could not initialize glew\n:");
        glfwTerminate();
        return -1;
    }
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    onGameStart();
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        onInput(window);

        onUpdate();

        glClear(GL_COLOR_BUFFER_BIT);
        onRender();
        glfwSwapBuffers(window);
    }
    onGameStop();
    glfwTerminate();
    return 0;
}