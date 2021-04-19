#include "game/game.h"

#include <raylib.h>
#include <rlgl.h>

#include "core/camera.h"
#include "core/map.h"

int game_main(Config initialConfig)
{
    Config cfg = initialConfig;
    InitWindow(cfg.render.width * cfg.render.upscaleMultiplier, cfg.render.height * cfg.render.upscaleMultiplier,
               GAME_WINDOW_TITLE);
    SetTargetFPS(60);

    // !This is temporary
    SetWindowPosition(1366, 0);

    RenderTexture2D target;
    if (cfg.render.upscaleMultiplier == 1) {
        target = LoadRenderTexture(cfg.render.width * cfg.render.upscaleMultiplier,
                                   cfg.render.height * cfg.render.upscaleMultiplier);
    } else {
        target = LoadRenderTexture(cfg.render.width, cfg.render.height);
    }

    DFCamera* cam = camera_create();
    DFMap* map = map_load();
    while (!WindowShouldClose()) {
        camera_update(cam, GetFrameTime());

        if (IsKeyPressed(KEY_F7))
            cfg.render.wireframe = !cfg.render.wireframe;
        if (IsKeyPressed(KEY_F8))
            map->model.materials[0].shader = map->affineShd;
        if (IsKeyPressed(KEY_F9))
            map->model.materials[0].shader = map->normalShd;

        BeginDrawing();

        BeginTextureMode(target);

        ClearBackground(cfg.skyColor);
        BeginMode3D(cam->rlCam);
        if (cfg.render.wireframe)
            rlEnableWireMode();
        map_render(map);
        if (cfg.render.wireframe)
            rlDisableWireMode();

        EndMode3D();

        EndTextureMode();

        DrawTexturePro(target.texture, (Rectangle){0, 0, target.texture.width, -target.texture.height},
                       (Rectangle){0, 0, cfg.render.width * cfg.render.upscaleMultiplier,
                                   cfg.render.height * cfg.render.upscaleMultiplier},
                       (Vector2){0, 0}, 0.0f, WHITE);
        EndDrawing();
    }
    map_unload(map);
    camera_destroy(cam);
    CloseWindow();
    return 0;
}