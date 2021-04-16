#define WINDOW_TITLE "The Deformers - A recreation of Terep2"
#define WINDOW_WIDTH 960
#define WINDOW_HEIGHT 600

#include "t_map.h"
#include "t_camera.h"
#include <raylib.h>

bool wireframe = false;

int main()
{
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE);
    SetTargetFPS(60);
    SetWindowPosition(1800, 300);
    TerepCamera* cam = camera_create();
    TerepMap* map = map_load();
    while (!WindowShouldClose()) {
        camera_update(cam, GetFrameTime());

        if (IsKeyPressed(KEY_F7))
            wireframe = !wireframe;

        BeginDrawing();
            ClearBackground(RAYWHITE);
            BeginMode3D(cam->rlCam);
                map_render(map, wireframe);
            EndMode3D();
        EndDrawing();
    }
    map_unload(map);
    camera_destroy(cam);
    CloseWindow();
    return 0;
}