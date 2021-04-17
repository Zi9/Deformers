#include "game/game.h"

#include <raylib.h>

#include "core/camera.h"
#include "core/map.h"


int game_main(Config initialConfig)
{
    Config cfg = initialConfig;
    InitWindow(cfg.renderer.width * cfg.renderer.upscaleMultiplier, cfg.renderer.height * cfg.renderer.upscaleMultiplier, GAME_WINDOW_TITLE);
    SetTargetFPS(60);
    SetWindowPosition(1800, 300);

    RenderTexture2D target;
    if (cfg.renderer.upscaleMultiplier == 1) {
        target = LoadRenderTexture(cfg.renderer.width * cfg.renderer.upscaleMultiplier, cfg.renderer.height * cfg.renderer.upscaleMultiplier);
    } else {
        target = LoadRenderTexture(cfg.renderer.width, cfg.renderer.height);
    }

    DFCamera* cam = camera_create();
    DFMap* map = map_load();
    while (!WindowShouldClose()) {
        camera_update(cam, GetFrameTime());

        if (IsKeyPressed(KEY_F7))
            cfg.renderer.wireframe = !cfg.renderer.wireframe;

        BeginDrawing();

        BeginTextureMode(target);

        ClearBackground(cfg.skyColor);
        BeginMode3D(cam->rlCam);

        map_render(map, cfg.renderer.wireframe);

        EndMode3D();

        EndTextureMode();

        DrawTexturePro(target.texture, (Rectangle){0, 0, target.texture.width, -target.texture.height},
                       (Rectangle){0, 0, cfg.renderer.width * cfg.renderer.upscaleMultiplier, cfg.renderer.height * cfg.renderer.upscaleMultiplier},
                       (Vector2){0, 0}, 0.0f, WHITE);
        EndDrawing();
    }
    map_unload(map);
    camera_destroy(cam);
    CloseWindow();
    return 0;
}