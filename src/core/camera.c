#include "camera.h"

#include <math.h>
#include <raymath.h>
#include <stdlib.h>

#define CAMERA_DEFAULT_MOUSE_SENS 0.2f
#define CAMERA_DEFAULT_SPEED 10.0f
#define CAMERA_SPEED_MOD_MULT 3

DFCamera* camera_create()
{
    DFCamera* cam = malloc(sizeof *cam);
    cam->freecamEnabled = false;

    cam->mouseSens = CAMERA_DEFAULT_MOUSE_SENS;
    cam->camSpeed = CAMERA_DEFAULT_SPEED;

    cam->yaw = -90.0f;
    cam->pitch = 0.0f;
    cam->worldUp = (Vector3){0.0f, 1.0f, 0.0f};
    cam->right = (Vector3){1.0f, 0.0f, 0.0f};
    cam->up = (Vector3){0.0f, 1.0f, 0.0f};
    cam->forward = (Vector3){0.0f, 0.0f, -1.0f};

    cam->mouseDelta = (Vector2){0.0f, 0.0f};

    cam->rlCam.position = (Vector3){0.0f, 0.0f, 0.0f};
    cam->rlCam.target = cam->forward;
    cam->rlCam.up = cam->up;
    cam->rlCam.fovy = 45.0f;
    cam->rlCam.type = CAMERA_PERSPECTIVE;
    return cam;
}
void camera_destroy(DFCamera* cam) { free(cam); }
void camera_enable_freecam(DFCamera* cam)
{
    cam->freecamEnabled = true;
    DisableCursor();
    cam->mousePos = GetMousePosition();
    cam->prevMousePos = GetMousePosition();
}
void camera_disable_freecam(DFCamera* cam)
{
    cam->freecamEnabled = false;
    EnableCursor();
}
void camera_update(DFCamera* cam, float dt)
{
    if (IsMouseButtonDown(MOUSE_MIDDLE_BUTTON)) {
        cam->rlCam.fovy += GetMouseWheelMove();
    }
    if (IsKeyPressed(KEY_F3)) {
        if (cam->freecamEnabled)
            camera_disable_freecam(cam);
        else
            camera_enable_freecam(cam);
    }
    if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON))
        camera_enable_freecam(cam);
    if (IsMouseButtonReleased(MOUSE_RIGHT_BUTTON))
        camera_disable_freecam(cam);

    if (cam->freecamEnabled) {
        cam->mousePos = GetMousePosition();
        cam->mouseDelta = Vector2Subtract(cam->mousePos, cam->prevMousePos);
        cam->prevMousePos = cam->mousePos;

        cam->yaw += cam->mouseDelta.x * cam->mouseSens;
        cam->pitch += -cam->mouseDelta.y * cam->mouseSens;
        if (cam->pitch > 89.0f)
            cam->pitch = 89.0f;
        else if (cam->pitch < -89.0f)
            cam->pitch = -89.0f;

        float vel = dt;
        if (IsKeyDown(KEY_LEFT_SHIFT))
            vel *= cam->camSpeed * CAMERA_SPEED_MOD_MULT;
        else if (IsKeyDown(KEY_LEFT_ALT))
            vel *= cam->camSpeed / CAMERA_SPEED_MOD_MULT;
        else
            vel *= cam->camSpeed;

        if (IsKeyDown(KEY_W))
            cam->rlCam.position =
                Vector3Add(cam->rlCam.position, Vector3Multiply(cam->forward, (Vector3){vel, vel, vel}));
        if (IsKeyDown(KEY_S))
            cam->rlCam.position =
                Vector3Subtract(cam->rlCam.position, Vector3Multiply(cam->forward, (Vector3){vel, vel, vel}));
        if (IsKeyDown(KEY_D))
            cam->rlCam.position =
                Vector3Add(cam->rlCam.position, Vector3Multiply(cam->right, (Vector3){vel, vel, vel}));
        if (IsKeyDown(KEY_A))
            cam->rlCam.position =
                Vector3Subtract(cam->rlCam.position, Vector3Multiply(cam->right, (Vector3){vel, vel, vel}));
        if (IsKeyDown(KEY_E))
            cam->rlCam.position = Vector3Add(cam->rlCam.position, Vector3Multiply(cam->up, (Vector3){vel, vel, vel}));
        if (IsKeyDown(KEY_Q))
            cam->rlCam.position =
                Vector3Subtract(cam->rlCam.position, Vector3Multiply(cam->up, (Vector3){vel, vel, vel}));

        cam->forward.x = cosf(DEG2RAD * cam->yaw) * cosf(DEG2RAD * cam->pitch);
        cam->forward.y = sinf(DEG2RAD * cam->pitch);
        cam->forward.z = sinf(DEG2RAD * cam->yaw) * cosf(DEG2RAD * cam->pitch);
        cam->forward = Vector3Normalize(cam->forward);
        cam->right = Vector3Normalize(Vector3CrossProduct(cam->forward, cam->worldUp));
        cam->up = Vector3Normalize(Vector3CrossProduct(cam->right, cam->forward));
        cam->rlCam.target = Vector3Add(cam->rlCam.position, cam->forward);
    }
}