#ifndef __MATH_CALC_H
#define __MATH_CALC_H

struct ProjectionResult {
    int x;
    int y;
};
struct ProjectionResult stereographic_projection_2D(int x, int y);
int calculate_angle_degrees(int x, int y);
int calculate_radius(int x, int y);
int scale_to_percent(int number, int scale_ref);

#endif
