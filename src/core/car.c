#include "car.h"

#include <stdio.h>
#include <stdlib.h>

#define CAR_PHYS_DEBUG_DRAW 1

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
void load_car_chunk1(DFCar* car, FILE* fp)
{
    fread(&(car->pointCount), sizeof car->pointCount, 1, fp);
    struct datapoint1 curPoint;
    for (size_t i = 0; i < car->pointCount; i++) {
        fread(&curPoint, sizeof curPoint, 1, fp);
        car->points[i].pos.x = curPoint.x / 100.0f;
        car->points[i].pos.y = curPoint.z / 100.0f;
        car->points[i].pos.z = curPoint.y / 100.0f;
        if (curPoint.diameter > 0) {
            car->points[i].diameter = curPoint.diameter / 100.0f;
        } else {
            car->points[i].diameter = 0.0f;
        }

        switch (curPoint.pointType) {
        case 0:
            car->points[i].type = DFCAR_POINT_GEOMETRY;
            break;
        case 1:
            if (curPoint.x < 0) {
                car->points[i].type = DFCAR_POINT_WHEEL_RL;
            } else {
                car->points[i].type = DFCAR_POINT_WHEEL_RR;
            }
            break;
        case 2:
            if (curPoint.x < 0) {
                car->points[i].type = DFCAR_POINT_WHEEL_FL;
            } else {
                car->points[i].type = DFCAR_POINT_WHEEL_FR;
            }
            break;
        case 65535:
            car->points[i].type = DFCAR_POINT_CAMERA;
            break;
        default:
            printf("Unknown type point: %i\n", curPoint.pointType);
        }
    }
    printf("INFO: CARLOAD: Loaded %d points\n", car->pointCount);
}

struct __attribute__((packed)) datapoint2 {
    uint16_t a, b;
    uint16_t stiffness, somephysvar1;
    uint16_t type;
    uint16_t somephysvar2, somephysvar3;
};
void load_car_chunk2(DFCar* car, FILE* fp)
{
    uint16_t count;
    fread(&count, sizeof count, 1, fp);
    car->physSegmentCount = 0;
    struct datapoint2 curPoint;
    for (size_t i = 0; i < count; i++) {
        fread(&curPoint, sizeof curPoint, 1, fp);
        car->physSegments[i].pointA = curPoint.a;
        car->physSegments[i].pointB = curPoint.b;
        switch (curPoint.type) {
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
            printf("Unknown type segment: %i\n", curPoint.type);
        }
        car->physSegmentCount++;
    }
    printf("INFO: CARLOAD: Loaded %d physics segments\n", car->physSegmentCount);
}

void load_car_chunk3(DFCar* car, FILE* fp)
{
    uint8_t dtype;
    while (fread(&dtype, sizeof dtype, 1, fp) != 0) {
        switch (dtype) {
        case 0:
            break;
        case 1:
            printf("\e[0;31m1:\t");
            int8_t data1[4];
            fread(&data1, sizeof(int8_t), 4, fp);
            for (size_t i = 0; i < 4; i++) {
                printf("%d\t", data1[i]);
            }
            break;
        case 3:
            printf("\e[0;32m3:\t");
            int16_t data3[6];
            fread(&data3, sizeof(int16_t), 6, fp);
            for (size_t i = 0; i < 6; i++) {
                printf("%d\t", data3[i]);
            }
            break;
        case 4:
            int16_t data4[8];
            uint8_t count4;
            fread(&count4, sizeof count4, 1, fp);
            printf("\e[0;33m4-%d:\t", count4);
            count4 += 2;
            if (count4 > 8) {
                printf("ChunkID4: Failure when parsing, too many data points\n");
                return;
            }
            fread(&data4, sizeof(int16_t), count4, fp);
            for (size_t i = 0; i < count4; i++) {
                printf("%d\t", data4[i]);
            }
            break;
        case 8:
            uint16_t data8[32];
            uint8_t count8;
            fread(&count8, sizeof count8, 1, fp);
            printf("\e[0;34m8-%d:\t", count8);
            count8 += 1;
            count8 *= 3;
            if (count8 > 32) {
                printf("ChunkID8: Failure when parsing, too many data points\n");
                return;
            }
            fread(&data8, sizeof(uint16_t), count8, fp);
            for (size_t i = 0; i < count8; i++) {
                printf("%d\t", data8[i]);
            }
            break;
        case 10:
            uint16_t data10[3];
            printf("\e[0;35m10:\t");
            fread(&data10, sizeof(uint16_t), 3, fp);
            for (size_t i = 0; i < 3; i++) {
                printf("%d\t", data10[i]);
            }
            break;
        case 69:
        case 246:
            uint8_t data246[19];
            printf("\e[0;36m246:\t");
            fread(&data246, sizeof(uint8_t), 19, fp);
            for (size_t i = 0; i < 19; i++) {
                printf("%d\t", data246[i]);
            }
            break;
            break;
        default:
            printf("Unknown data block %d", dtype);
            return;
        }
        if (dtype != 0) {
            // printf("- @ %ld\n", ftell(fp) - 1);
            printf("\n");
        }
    }
    printf("\e[0mINFO: CARLOAD: Reached end of car data file\n");
}

struct __attribute__((packed)) carDatHeader {
    uint16_t chunk1Start;
    uint16_t chunk2Start;
    uint16_t chunk3Start;
    uint16_t unknown;
    uint16_t drivetrainMode;
};
DFCar* car_load(const char* path)
{
    DFCar* car = malloc(sizeof *car);
    FILE* f = fopen(path, "r");

    struct carDatHeader hdr;
    fread(&hdr, sizeof hdr, 1, f);
    car->drivetrainMode = hdr.drivetrainMode;
    switch (car->drivetrainMode)
    {
        case DFCAR_DRIVETRAIN_RWD:
            printf("INFO: CARLOAD: Car is RWD\n");
            break;
        case DFCAR_DRIVETRAIN_FWD:
            printf("INFO: CARLOAD: Car is FWD\n");
            break;
        case DFCAR_DRIVETRAIN_AWD:
            printf("INFO: CARLOAD: Car is AWD\n");
            break;
        default:
            printf("INFO: CARLOAD: Unknown drivetrain mode!\n");
    }
    fseek(f, hdr.chunk1Start, SEEK_SET);
    load_car_chunk1(car, f);
    load_car_chunk2(car, f);
    load_car_chunk3(car, f);

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
        if (car->currentSelSeg == i) {
            col = PURPLE;
        } else {
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
        }
        DrawLine3D(car->points[car->physSegments[i].pointA].pos, car->points[car->physSegments[i].pointB].pos, col);
    }
#endif
}