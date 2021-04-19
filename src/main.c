#include "core/config.h"
#include "game/game.h"

int main()
{
    Config cfg = config_get_default();
    cfg.render.width = 640;
    cfg.render.height = 360;
    cfg.render.upscaleMultiplier = 2;
    game_main(cfg);
}