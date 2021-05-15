#pragma once

#include <stdint.h>
#include <raylib.h>

enum PointType {
    DFCAR_POINT_GEOMETRY,
    DFCAR_POINT_WHEEL_FL,
    DFCAR_POINT_WHEEL_FR,
    DFCAR_POINT_WHEEL_RL,
    DFCAR_POINT_WHEEL_RR,
    DFCAR_POINT_CAMERA
};
typedef struct DFCarPoint {
    Vector3 pos;
    uint8_t type;
} DFCarPoint;

typedef struct DFCar {
    uint16_t pointCount;
    DFCarPoint points[128];

    uint16_t testcount;
    uint16_t a[256];
    uint16_t b[256];
} DFCar;

DFCar* car_load(const char* path);
void car_unload(DFCar* car);
void car_render(DFCar* car);