#ifndef __PATH_CHECK_H
#define __PATH_CHECK_H
#include <stdio.h>

//(FENRIS24) datatype for kjørekommando etter path_check. Inneholder 2 int (vinkel og fart) og en bool (om joystick er trykket inn eller ikke)
struct path_check_results {
    int angle;
    int speed;
    bool js_num;
};

//(FENRIS24) Funksjonen som sammenligner ønsket kjøreretning med målte sensorverdier. Kjørekommando sin fart setter lik 0 hvis ønsket kjøreretning er blokkert.
struct path_check_results path_check(int angle, int speed, bool js_num, uint8_t sensorValues);

#endif