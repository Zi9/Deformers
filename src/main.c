#include "game/game.h"
#include "core/config.h"

int main()
{
    game_main(config_get_default());
}