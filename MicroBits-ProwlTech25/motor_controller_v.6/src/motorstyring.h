#ifndef MOTORSTYRING_H
#define MOTORSTYRING_H

// Struktur som holder output‐verdier for de fire hjulene
struct MotorVerdier {
    float front_left;
    float front_right;
    float rear_left;
    float rear_right;
};

// Kalkulerer motor‐verdier for gitt fremdrift, vinkel, rotasjon + sving_js
struct MotorVerdier kalkulerMotorVerdier(
    float fart,
    float vinkel,
    float rotasjon,
    float sving_js
);

// Høyere nivå funksjon som tar inn joystick‐data og sender RPM til hjul
int kontroller_motorene(
    float fart,
    float vinkel,
    float rotasjon,
    float sving_js
);

#endif // MOTORSTYRING_H
