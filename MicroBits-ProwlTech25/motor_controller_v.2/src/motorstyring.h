#ifndef MOTORSTYRING_H
#define MOTORSTYRING_H

struct MotorVerdier {
    float front_left;
    float front_right;
    float rear_left;
    float rear_right;
};

struct MotorVerdier kalkulerMotorVerdier(float fart, float vinkel, float rotasjon);
int kontroller_motorene(float fart, float vinkel, float rotasjon);


#endif //MOTORSTYRING_H


