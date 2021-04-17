#pragma once

#include <raylib.h>
#include <stdint.h>

#define GAME_WINDOW_TITLE "Deformers - A recreation of Terep2"
#define EDITOR_WINDOW_TITLE "Deformers - A recreation of Terep2 - Editor Mode"

typedef enum {
    MODE_GAME,
    MODE_EDITOR
} StartMode;

struct RenderConfig {
    uint16_t width;
    uint16_t height;
    uint8_t upscaleMultiplier;
    bool wireframe;
};

typedef struct Config {
    struct RenderConfig render;
    char* dataPath;
    Color skyColor;
    bool restart;
    StartMode mode;
} Config;

Config config_get_default();