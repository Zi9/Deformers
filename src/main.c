#define WINDOW_TITLE "The Deformers - A recreation of Terep2"
#define WINDOW_WIDTH 960
#define WINDOW_HEIGHT 600

#include <raylib.h>
#include "t_map.h"

int main()
{
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE);
    SetTargetFPS(30);
    SetWindowPosition(1800, 300);
    TerepMap* map = map_load();
    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        map_render(map);
        DrawText("Hello world", 0, 0, 20, BLACK);
        EndDrawing();
    }
    map_unload(map);
    CloseWindow();
    return 0;
}