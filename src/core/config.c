#include "core/config.h"

Config config_terep()
{
    // clang-format off
    Config cfg = {
        .render = {
            .width = 320,
            .height = 200,
            .upscaleMultiplier = 2},
        .baseDataPath = "./data/terep/",
        .scenario = "terep",
        .skyColor = (Color){93, 199, 255, 255},
        .restart = false,
    };
    // clang-format on
    return cfg;
}

Config config_default()
{
    Config cfg = config_terep();
    cfg.render.width = 1280;
    cfg.render.height = 720;
    cfg.render.upscaleMultiplier = 1;
    return cfg;
}
