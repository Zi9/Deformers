#pragma once

#include <stdint.h>
#include <raylib.h>

typedef struct DFCar {
    Vector3 wheel_fl;
    Vector3 wheel_fr;
    Vector3 wheel_rl;
    Vector3 wheel_rr;

    Vector3 driverView;

    uint16_t geoPointCount;
    Vector3 geoPoints[128];
} DFCar;

DFCar* car_load(const char* path);
void car_unload(DFCar* car);
void car_render(DFCar* car);