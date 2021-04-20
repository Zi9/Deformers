#include "core/config.h"
#include "game/game.h"

int main(int argc, char* argv[])
{
    Config cfg = config_get_default();
    cfg.render.width = 640;
    cfg.render.height = 360;
    cfg.render.upscaleMultiplier = 2;
    cfg.dataPath = "../data/";
    do {
        cfg = game_main(cfg);
    } while (cfg.restart == true);
}