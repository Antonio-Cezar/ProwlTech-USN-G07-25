#ifndef MOTORSTYRING_H
#define MOTORSTYRING_H

//MOTORSTYRING.H ER SKREVET AV RAMEEZ SADIQ (PROWLTECH2025)
// Struct som holder output‐verdier for de fire hjulene
struct MotorVerdier {
    float foran_venstre;
    float foran_høyre;
    float bak_venstre;
    float bak_høyre;
};

// Kalkulerer motor‐verdier for fart, vinkel, rotasjon og sving_js
struct MotorVerdier kalkulerMotorVerdier(
    float fart,
    float vinkel,
    float rotasjon,
    float sving_js
);

// funksjon som tar inn joystick‐data og sender RPM til hjul
int kontroller_motorene(
    float fart,
    float vinkel,
    float rotasjon,
    float sving_js
);

#endif // MOTORSTYRING_H
