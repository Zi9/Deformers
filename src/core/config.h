#pragma once

#include <raylib.h>
#include <stdint.h>

#define GAME_WINDOW_TITLE "Deformers - A recreation of Terep2"
// #define EDITOR_WINDOW_TITLE "Deformers - A recreation of Terep2 - Editor Mode"

typedef struct Config {
    struct {
        uint16_t width;
        uint16_t height;
        uint8_t upscaleMultiplier;
    } render;
    char* baseDataPath;
    char* scenario;
    Color skyColor;
    bool restart;
} Config;

Config config_terep();
Config config_default();