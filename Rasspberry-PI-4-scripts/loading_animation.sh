#!/bin/bash

# Enkel laster-animasjon med roterende symboler
loading_animasjon() {
    local i=0
    local symboler=('|' '/' '-' '\\')
    echo -n "Laster neste modul "
    while [ $i -lt 10 ]; do
        for s in "${symboler[@]}"; do
            echo -ne "\rLaster neste modul $s"
            sleep 0.2
        done
        ((i++))
    done
    echo -e "\rModul klar!         "
}

loading_animasjon
