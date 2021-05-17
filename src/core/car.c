#include "car.h"

#include <stdio.h>
#include <stdlib.h>

#define CAR_PHYS_DEBUG_DRAW 0

struct __attribute__((packed)) datapoint1 {
    uint16_t pad1;
    int16_t x;
    uint16_t pad2;
    int16_t y;
    uint16_t pad3;
    int16_t z;
    uint8_t pad4[12];
    int16_t diameter;
    uint16_t pointType;
};
void load_car_chunk1(DFCar* car, uint8_t* buf, uint16_t start)
{
    uint16_t count = *(buf + start);
    car->pointCount = count;
#define CHUNKSZ 28
    for (size_t i = 0; i < count; i++) {
        struct datapoint1* curPoint = (struct datapoint1*)(buf + start + 2 + (i * CHUNKSZ));
        car->points[i].pos.x = curPoint->x / 100.0f;
        car->points[i].pos.y = curPoint->z / 100.0f;
        car->points[i].pos.z = curPoint->y / 100.0f;
        if (curPoint->diameter > 0) {
            car->points[i].diameter = curPoint->diameter / 100.0f;
        } else {
            car->points[i].diameter = 0.0f;
        }

        switch (curPoint->pointType) {
        case 0:
            car->points[i].type = DFCAR_POINT_GEOMETRY;
            break;
        case 1:
            if (curPoint->x < 0) {
                car->points[i].type = DFCAR_POINT_WHEEL_RL;
            } else {
                car->points[i].type = DFCAR_POINT_WHEEL_RR;
            }
            break;
        case 2:
            if (curPoint->x < 0) {
                car->points[i].type = DFCAR_POINT_WHEEL_FL;
            } else {
                car->points[i].type = DFCAR_POINT_WHEEL_FR;
            }
            break;
        case 65535:
            car->points[i].type = DFCAR_POINT_CAMERA;
            break;
        default:
            printf("Unknown type point: %i\n", curPoint->pointType);
        }
    }
#undef CHUNKSZ
}

struct __attribute__((packed)) datapoint2 {
    uint16_t a, b;
    uint16_t stiffness, somephysvar1;
    uint16_t type;
    uint16_t somephysvar2, somephysvar3;
};
void load_car_chunk2(DFCar* car, uint8_t* buf, uint16_t start)
{
    uint16_t count = *(buf + start);
    car->physSegmentCount = 0;
#define CHUNKSZ 14
    for (size_t i = 0; i < count; i++) {
        struct datapoint2* curPoint = (struct datapoint2*)(buf + start + 2 + (i * CHUNKSZ));
        car->physSegments[i].pointA = curPoint->a;
        car->physSegments[i].pointB = curPoint->b;
        switch (curPoint->type) {
        case 0:
            car->physSegments[i].type = DFCAR_SEGMENT_SUSP_EXTRA;
            break;
        case 1:
            car->physSegments[i].type = DFCAR_SEGMENT_NORMAL;
            break;
        case 4:
        case 6:
            car->physSegments[i].type = DFCAR_SEGMENT_SUSP_REAR;
            break;
        case 10:
        case 12:
            car->physSegments[i].type = DFCAR_SEGMENT_SUSP_FRONT;
            break;
        default:
            printf("Unknown type segment: %i\n", curPoint->type);
        }
        car->physSegmentCount++;
    }
#undef CHUNKSZ
}

struct __attribute__((packed)) carDatHeader {
    uint16_t chunk1Start;
    uint16_t chunk2Start;
    uint16_t chunk3Start;
};
DFCar* car_load(const char* path)
{
    DFCar* car = malloc(sizeof *car);
    FILE* f = fopen(path, "r");
    fseek(f, 0, SEEK_END);
    long fileSZ = ftell(f);
    fseek(f, 0, SEEK_SET);
    uint8_t* buf = malloc(fileSZ);
    fread(buf, fileSZ, 1, f);

    struct carDatHeader* hdr = (struct carDatHeader*)buf;
    load_car_chunk1(car, buf, hdr->chunk1Start);
    load_car_chunk2(car, buf, hdr->chunk2Start);
    free(buf);

    fclose(f);
    return car;
}
void car_unload(DFCar* car) { free(car); }
void car_render(DFCar* car)
{
    Color col;
    for (size_t i = 0; i < car->pointCount; i++) {
        switch (car->points[i].type) {
        case DFCAR_POINT_GEOMETRY:
            col = BLACK;
            break;
        case DFCAR_POINT_CAMERA:
            col = MAGENTA;
            break;
        case DFCAR_POINT_WHEEL_FL:
        case DFCAR_POINT_WHEEL_FR:
            col = BLUE;
            break;
        case DFCAR_POINT_WHEEL_RL:
        case DFCAR_POINT_WHEEL_RR:
            col = RED;
            break;
        }
        DrawCube(car->points[i].pos, 0.05f, 0.05f, 0.05f, col);
#if CAR_PHYS_DEBUG_DRAW == 1
        if (car->points[i].diameter > 0) {
            DrawCircle3D(car->points[i].pos, car->points[i].diameter, (Vector3){0.0f, 1.0f, 0.0f}, 90, PINK);
        }
#endif
    }
#if CAR_PHYS_DEBUG_DRAW == 1
    for (size_t i = 0; i < car->physSegmentCount; i++) {
        switch (car->physSegments[i].type) {
        case DFCAR_SEGMENT_NORMAL:
            col = WHITE;
            break;
        case DFCAR_SEGMENT_SUSP_FRONT:
            col = BLUE;
            break;
        case DFCAR_SEGMENT_SUSP_REAR:
            col = RED;
            break;
        case DFCAR_SEGMENT_SUSP_EXTRA:
            col = GREEN;
            break;
        }
        DrawLine3D(car->points[car->physSegments[i].pointA].pos, car->points[car->physSegments[i].pointB].pos, col);
    }
#endif
}