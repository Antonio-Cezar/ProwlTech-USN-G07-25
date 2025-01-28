#ifndef __PATH_CHECK_H
#define __PATH_CHECK_H
#include <stdio.h>


struct path_check_results {
    int angle;
    int speed;
    bool js_num;
};

struct path_check_results path_check(int angle, int speed, bool js_num, uint8_t sensorValues);

#endif