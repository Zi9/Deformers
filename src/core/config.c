#include "core/config.h"

#include <stdint.h>

Config config_get_default()
{
    Config cfg = {
        .renderer = {
            .width = 320,
            .height = 200,
            .upscaleMultiplier = 2
        },
        .dataPath = "./",
        .skyColor = (Color) {93, 199, 255, 255}
    };
    return cfg;
}