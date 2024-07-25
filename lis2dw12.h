
#pragma once

#include <time.h>
#include <zephyr/device.h>

typedef union
{
    double value[3];
    struct
    {
        double x, y, z;
    };
} drv_acc_value_t;

int acc_init(void);
int acc_get(drv_acc_value_t* p_value);
void acc_get_data(int index, drv_acc_value_t* p_value);




