#pragma once

#include <raylib.h>

typedef struct DFCamera {
    float mouseSens, camSpeed;
    float yaw, pitch;
    Vector3 worldUp, right, up, forward;
    Vector2 mousePos, prevMousePos, mouseDelta;
    bool freecamEnabled;
    Camera3D rlCam;
} DFCamera;

DFCamera camera_create();
void camera_enable_freecam(DFCamera* cam);
void camera_disable_freecam(DFCamera* cam);
void camera_update(DFCamera* cam, float dt);