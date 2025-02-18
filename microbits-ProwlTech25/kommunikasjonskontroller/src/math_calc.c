#include <math.h>
#include "math_calc.h"

struct ProjectionResult stereographic_projection_2D(int x, int y){
    double a = (double)y / (double)x;
    double temp_x = 1 / sqrt(1 + (a * a));
    double temp_y = a * temp_x;
    
    struct ProjectionResult result;
    result.x = (int)(temp_x * x);
    result.y = (int)(temp_y * y);

    if ((y > 29 && result.y < 29) || (y < -28 && result.y > 29)){
        result.y = (result.y * -1);
    }
    
    // Return the struct
    return result;
}

int calculate_angle_degrees(int x, int y){
    int temp_x = x;
    int temp_y = y;
    if ((-40 < x) && (x < 41)){
        temp_x = 0;
    }
    if ((-28 < y) && (y < 29)){
        temp_y = 0;
    }
    if (x > 1820){temp_x = 1820;}
    if (x < -1820){temp_x = -1820;}
    if (y > 1820){temp_y = 1820;}
    if (y < -1820){temp_y = -1820;}

    // temp_x = temp_x / 1820;
    // temp_y = temp_y / 1820;

    double angle_radians = atan2(temp_x, temp_y);
    double angle_degrees = ((angle_radians * 180) / 3.14159);

    if (angle_degrees < 0){angle_degrees += 360.0;}

    return (int)angle_degrees;
}

int calculate_radius(int x, int y){
    int temp_x = x;
    int temp_y = y;
    if ((-40 < x) && (x < 40)){
        temp_x = 0;
    }
    if ((-40 < y) && (y < 40)){
        temp_y = 0;
    }
    double radius = sqrt((temp_x * temp_x) + (temp_y * temp_y));

    return (int)radius;
}

int scale_to_percent(int number, int max_value){
    double scaled_number = ((double)number / (double)max_value) * 100.0;
    return (int)(scaled_number);
}
