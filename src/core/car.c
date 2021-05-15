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
    int32_t x, y, z;
    uint8_t misc[16];
};

enum PointType {
    POINT_GEOMETRY = 0,
    POINT_WHEEL_REAR = 1,
    POINT_WHEEL_FRONT = 2,
    POINT_CAMERA = 65535
};

void load_car_chunk1(DFCar* car, uint8_t* buf, uint16_t start)
{
    uint16_t pointcount = *(buf + start);
    car->geoPointCount = 0;
    #define CHUNKSZ 28
    for (size_t i = 0; i < pointcount; i++)
    {
        struct datapoint1* curPoint = (struct datapoint1*)(buf + start + 2 + (i * CHUNKSZ));
        switch (curPoint->pointType)
        {
            case POINT_GEOMETRY:
                car->geoPoints[car->geoPointCount].x = curPoint->x/100.0f;
                car->geoPoints[car->geoPointCount].y = curPoint->z/100.0f;
                car->geoPoints[car->geoPointCount].z = curPoint->y/100.0f;
                car->geoPointCount++;
                break;
            case POINT_CAMERA:
                car->driverView.x = curPoint->x/100.0f;
                car->driverView.y = curPoint->z/100.0f;
                car->driverView.z = curPoint->y/100.0f;
                break;
            case POINT_WHEEL_FRONT:
                if (curPoint->x < 0) {
                    car->wheel_fl.x = curPoint->x/100.0f;
                    car->wheel_fl.y = curPoint->z/100.0f;
                    car->wheel_fl.z = curPoint->y/100.0f;
                } else {
                    car->wheel_fr.x = curPoint->x/100.0f;
                    car->wheel_fr.y = curPoint->z/100.0f;
                    car->wheel_fr.z = curPoint->y/100.0f;
                }
                break;
            case POINT_WHEEL_REAR:
                if (curPoint->x < 0) {
                    car->wheel_rl.x = curPoint->x/100.0f;
                    car->wheel_rl.y = curPoint->z/100.0f;
                    car->wheel_rl.z = curPoint->y/100.0f;
                } else {
                    car->wheel_rr.x = curPoint->x/100.0f;
                    car->wheel_rr.y = curPoint->z/100.0f;
                    car->wheel_rr.z = curPoint->y/100.0f;
                }
                break;
        }
    }
}

struct __attribute__((packed)) carDatHeader {
    uint16_t pointsStart;
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
    load_car_chunk1(car, buf, hdr->pointsStart);
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
    for (size_t i = 0; i < car->geoPointCount; i++)
    {
        DrawCube(car->geoPoints[i], 0.05f, 0.05f, 0.05f, BLACK);
    }
    DrawCube(car->driverView, 0.05f, 0.05f, 0.05f, GREEN);
    DrawCube(car->wheel_fl, 0.05f, 0.05f, 0.05f, RED);
    DrawCube(car->wheel_fr, 0.05f, 0.05f, 0.05f, RED);
    DrawCube(car->wheel_rl, 0.05f, 0.05f, 0.05f, BLUE);
    DrawCube(car->wheel_rr, 0.05f, 0.05f, 0.05f, BLUE);
}