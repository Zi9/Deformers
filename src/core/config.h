#pragma once

#include <raylib.h>
#include <stdint.h>

#define GAME_WINDOW_TITLE "Deformers - A recreation of Terep2"
#define EDITOR_WINDOW_TITLE "Deformers - A recreation of Terep2 - Editor Mode"

struct RendererConfig {
    uint16_t width;
    uint16_t height;
    uint8_t upscaleMultiplier;
};

typedef struct Config {
    struct RendererConfig renderer;
    char* dataPath;
    Color skyColor;
} Config;

Config config_get_default();