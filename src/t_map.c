#include "t_map.h"

#include <stdlib.h>

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

const GLfloat tempdatabuf[] = {
    -0.8f, 0.8f, 0.0f, 0.0f, 1.0f, -0.8f, -0.8f, 0.0f, 1.0f, 1.0f, 0.8f, 0.8f,  0.0f, 0.0f, 0.0f,

    0.8f,  0.8f, 0.0f, 1.0f, 1.0f, -0.8f, -0.8f, 0.0f, 0.0f, 0.0f, 0.8f, -0.8f, 0.0f, 1.0f, 0.0f,
};

struct Map* map_load()
{
    struct Map* map = malloc(sizeof *map);
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
    // map->shader = compile_shader(mapVertexShader, mapFragmentShader);

    // glGenTextures(1, &map->glTex);
    // glBindTexture(GL_TEXTURE_2D, map->glTex);
    // glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, map->texture->width, map->texture->height, 0, GL_RGBA8,
    // GL_UNSIGNED_BYTE, map->texture->pixels); glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
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