#pragma once

#include <raylib.h>

typedef struct TerepCamera {
    float mouseSens, camSpeed;
    float yaw, pitch;
    Vector3 worldUp, right, up, forward;
    Vector2 mousePos, prevMousePos, mouseDelta;
    bool freecamEnabled;
    Camera3D rlCam;
} TerepCamera;

TerepCamera* camera_create();
void camera_destroy(TerepCamera* cam);
void camera_enable_freecam(TerepCamera* cam);
void camera_disable_freecam(TerepCamera* cam);
void camera_update(TerepCamera* cam, float dt);