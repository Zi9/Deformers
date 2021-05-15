#include "car.h"

#include <stdio.h>
#include <stdlib.h>

struct __attribute__((packed)) datapoint1 {
    uint16_t pad1;
    int16_t x;
    uint16_t pad2;
    int16_t y;
    uint16_t pad3;
    int16_t z;
    uint8_t pad4[12];
    int16_t flag1;
    uint16_t pointType;
};
struct __attribute__((packed)) datapoint2 {
    uint16_t a, b;
    uint8_t misc[10];
};

void load_car_chunk1(DFCar* car, uint8_t* buf, uint16_t start)
{
    uint16_t pointcount = *(buf + start);
    car->pointCount = pointcount;
    #define CHUNKSZ 28
    for (size_t i = 0; i < pointcount; i++)
    {
        struct datapoint1* curPoint = (struct datapoint1*)(buf + start + 2 + (i * CHUNKSZ));
        car->points[i].pos.x = curPoint->x/100.0f;
        car->points[i].pos.y = curPoint->z/100.0f;
        car->points[i].pos.z = curPoint->y/100.0f;

        switch (curPoint->pointType)
        {
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
        }
    }
    #undef CHUNKSZ
}
void load_car_chunk2(DFCar* car, uint8_t* buf, uint16_t start)
{
    uint16_t pointcount = *(buf + start);
    car->testcount = 0;
    #define CHUNKSZ 14
    for (size_t i = 0; i < pointcount; i++)
    {
        struct datapoint2* curPoint = (struct datapoint2*)(buf + start + 2 + (i * CHUNKSZ));
        car->a[i] = curPoint->a;
        car->b[i] = curPoint->b;
        car->testcount++;
    }
    #undef CHUNKSZ
}

struct __attribute__((packed)) carDatHeader {
    uint16_t pointsStart;
    uint16_t phyChkStart;
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
    load_car_chunk1(car, buf, hdr->pointsStart);
    load_car_chunk2(car, buf, hdr->phyChkStart);
    free(buf);

    fclose(f);
    return car;
}
void car_unload(DFCar* car)
{
    free(car);
}
void car_render(DFCar* car)
{
    for (size_t i = 0; i < car->pointCount; i++)
    {
        DrawCube(car->points[i].pos, 0.05f, 0.05f, 0.05f, BLACK);
    }
    for (size_t i = 0; i < car->testcount; i++)
    {
        DrawLine3D(car->points[car->a[i]].pos, car->points[car->b[i]].pos, WHITE);
    }
}