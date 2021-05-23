#include "game/game.h"

#include <raylib.h>
#include <rlgl.h>

#include "core/camera.h"
#include "core/car.h"
#include "core/map.h"

Config game_main(Config initialConfig)
{
    Config cfg = initialConfig;
    InitWindow(cfg.render.width * cfg.render.upscaleMultiplier, cfg.render.height * cfg.render.upscaleMultiplier,
               GAME_WINDOW_TITLE);
    SetTargetFPS(60);
    SetExitKey(KEY_CAPS_LOCK);

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
    DFMap* map = map_load(cfg.dataPath);
    DFCar* car = car_load("../data/car1.dat");
    car->currentSelSeg = 0;
    while (!WindowShouldClose()) {
        camera_update(cam, GetFrameTime());

        // if (IsKeyPressed(KEY_F1))
        //     cfg.restart = false;

        if (IsKeyPressed(KEY_F7))
            cfg.render.wireframe = !cfg.render.wireframe;
        if (IsKeyPressed(KEY_RIGHT))
            car->currentSelSeg++;
        if (IsKeyPressed(KEY_LEFT))
            car->currentSelSeg--;
        // if (IsKeyPressed(KEY_F8))
        //     map->model.materials[0].shader = map->affineShd;
        // if (IsKeyPressed(KEY_F9))
        //     map->model.materials[0].shader = map->normalShd;

        BeginDrawing();
        BeginTextureMode(target);
        ClearBackground(cfg.skyColor);
        BeginMode3D(cam->rlCam);

        if (cfg.render.wireframe)
            rlEnableWireMode();
        map_render(map);
        car_render(car);
        if (cfg.render.wireframe)
            rlDisableWireMode();

        EndMode3D();
        EndTextureMode();

        DrawTexturePro(target.texture, (Rectangle){0, 0, target.texture.width, -target.texture.height},
                       (Rectangle){0, 0, cfg.render.width * cfg.render.upscaleMultiplier,
                                   cfg.render.height * cfg.render.upscaleMultiplier},
                       (Vector2){0, 0}, 0.0f, WHITE);
        DrawText(TextFormat("CUR SEG: %i", car->currentSelSeg), 0, 0, 20, RED);
        EndDrawing();
    }
    map_unload(map);
    car_unload(car);
    camera_destroy(cam);
    CloseWindow();
    return cfg;
}
