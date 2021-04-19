#include "core/config.h"

#include <stdint.h>

Config config_get_default()
{
    // clang-format off
    Config cfg = {
        .render = {
            .width = 320,
            .height = 200,
            .upscaleMultiplier = 2,
            .wireframe = false},
        .dataPath = "./",
        .skyColor = (Color){93, 199, 255, 255},
        .restart = false,
        .mode = MODE_GAME
    };
    // clang-format on
    return cfg;
}