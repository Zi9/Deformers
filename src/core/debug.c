#include "debug.h"

#include <stdio.h>

void print_hexdump(void* buf, uint32_t count, uint32_t bytes_per_line)
{
    printf("\n");
    for (uint32_t counter = 0; counter < count; counter++)
    {
        printf("%02X ", *((uint8_t*)buf + counter));
        if ((counter+1) % bytes_per_line == 0)
        {
            printf("\n");
        }
    }
    printf("\n");
}