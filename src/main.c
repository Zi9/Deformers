#define WINDOW_TITLE "Deformers - A recreation of Terep2"
#define RENDER_SIZE_MULTIPLIER 2
#define RENDER_WIDTH 320
#define RENDER_HEIGHT 200
#define RENDER_PIXELIZED true

#define TEREP_SKY                                                                                                      \
    (Color) { 93, 199, 255, 255 }

#include "t_camera.h"
#include "t_map.h"
#include <raylib.h>

bool wireframe = false;

int main()
{
    InitWindow(RENDER_WIDTH * RENDER_SIZE_MULTIPLIER, RENDER_HEIGHT * RENDER_SIZE_MULTIPLIER, WINDOW_TITLE);
    SetTargetFPS(60);
    SetWindowPosition(1800, 300);

    RenderTexture2D target;
    if (RENDER_PIXELIZED) {
        target = LoadRenderTexture(RENDER_WIDTH, RENDER_HEIGHT);
    } else {
        target = LoadRenderTexture(RENDER_WIDTH * RENDER_SIZE_MULTIPLIER, RENDER_HEIGHT * RENDER_SIZE_MULTIPLIER);
    }

    TerepCamera* cam = camera_create();
    TerepMap* map = map_load();
    while (!WindowShouldClose()) {
        camera_update(cam, GetFrameTime());

        if (IsKeyPressed(KEY_F7))
            wireframe = !wireframe;

        BeginDrawing();

        BeginTextureMode(target);

        ClearBackground(TEREP_SKY);
        BeginMode3D(cam->rlCam);

        map_render(map, wireframe);

        EndMode3D();

        EndTextureMode();

        DrawTexturePro(target.texture, (Rectangle){0, 0, target.texture.width, -target.texture.height},
                       (Rectangle){0, 0, RENDER_WIDTH * RENDER_SIZE_MULTIPLIER, RENDER_HEIGHT * RENDER_SIZE_MULTIPLIER},
                       (Vector2){0, 0}, 0.0f, WHITE);
        EndDrawing();
    }
    map_unload(map);
    camera_destroy(cam);
    CloseWindow();
    return 0;
}