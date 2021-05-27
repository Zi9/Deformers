#include "core/config.h"
#include "game/game.h"

// int main(int argc, char* argv[])
int main()
{
    Config cfg = config_default();
    do {
        cfg = game_main(cfg);
    } while (cfg.restart == true);
}