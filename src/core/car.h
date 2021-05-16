#pragma once

#include <raylib.h>
#include <stdint.h>

enum PointType {
    DFCAR_POINT_GEOMETRY,
    DFCAR_POINT_WHEEL_FL,
    DFCAR_POINT_WHEEL_FR,
    DFCAR_POINT_WHEEL_RL,
    DFCAR_POINT_WHEEL_RR,
    DFCAR_POINT_CAMERA
};
enum SegmentType {
    DFCAR_SEGMENT_NORMAL,
    DFCAR_SEGMENT_SUSP_FRONT,
    DFCAR_SEGMENT_SUSP_REAR,
    DFCAR_SEGMENT_SUSP_EXTRA,
};
typedef struct DFCarPoint {
    Vector3 pos;
    uint8_t type;
    float diameter;
} DFCarPoint;
typedef struct DFCarPhysSegment {
    uint16_t pointA;
    uint16_t pointB;
    uint8_t type;
} DFCarPhysSegment;

typedef struct DFCar {
    uint16_t pointCount;
    DFCarPoint points[128];

    uint16_t physSegmentCount;
    DFCarPhysSegment physSegments[256];
} DFCar;

DFCar* car_load(const char* path);
void car_unload(DFCar* car);
void car_render(DFCar* car);