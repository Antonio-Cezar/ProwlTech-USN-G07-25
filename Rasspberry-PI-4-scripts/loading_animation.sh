#!/bin/bash

# Enkel laster-animasjon med roterende symboler
loading_animasjon() {
    local i=0
    local symboler=('|' '/' '-' '\\')
    echo -n "Laster  "
    while [ $i -lt 10 ]; do
        for s in "${symboler[@]}"; do
            echo -ne "\rLaster $s"
            sleep 0.1
        done
        ((i++))
    done
    echo -e "\rModul klar!         "
}

loading_animasjon
