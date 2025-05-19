//Laget av RAS (husk å kommentere)
#ifndef MOTORSTYRING_H
#define MOTORSTYRING_H
#include <stdbool.h>


struct MotorVerdier {
    float front_left;
    float front_right;
    float rear_left;
    float rear_right;
};

struct MotorVerdier kalkulerMotorVerdier(float fart, float vinkel, float rotasjon);
int kontroller_motorene(float fart, float vinkel, float rotasjon);
void sett_nødstopp(bool aktivert);
bool er_nødstopp_aktivert();

int speed_to_ds(int duty_cycle);
int mapValue(int prosent, int retning);


#endif //MOTORSTYRING_H


