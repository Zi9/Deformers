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
enum DrivetrainMode {
    DFCAR_DRIVETRAIN_RWD = 0,
    DFCAR_DRIVETRAIN_FWD = 1,
    DFCAR_DRIVETRAIN_AWD = 2,
};
typedef struct DFCarPoint {
    Vector3 pos;
    uint8_t type;
    float diameter;
} DFCarPoint;
typedef struct DFCarPhysSegment {
    uint16_t pointA, pointB;
    uint8_t type;
    uint16_t other1, other2, other3, other4;
} DFCarPhysSegment;

typedef struct DFCar {
    uint16_t pointCount;
    DFCarPoint points[128];

    uint16_t physSegmentCount;
    DFCarPhysSegment physSegments[256];
    uint16_t currentSelSeg;

    uint8_t drivetrainMode;
} DFCar;

DFCar* car_load(const char* path);
void car_unload(DFCar* car);
void car_render(DFCar* car);