#ifndef __MATH_CALC_H
#define __MATH_CALC_H
// Datatype for variabel som skal holde x og y verdi etter stereografisk projeksjon - Fenri24
struct ProjectionResult {
    int x;
    int y;
};
// Funksjon som omgjør akser i en firkant til akse-verdier på en sirkel - Fenri24
struct ProjectionResult stereographic_projection_2D(int x, int y);

// Funksjon som regner vinkel til punkt(x,y) fra origo - Fenri24
int calculate_angle_degrees(int x, int y);

// Funksjon som regner avstanden fra punkt(x,y) til origo - Fenri24
int calculate_radius(int x, int y);

// Funksjon som skalerer et tall iforhold til en referanse verdi - Fenri24
int scale_to_percent(int number, int scale_ref);
#endif
