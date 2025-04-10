#!/bin/bash

loading_animasjon() {
    local symboler=('|' '/' '-' '\\')
    echo -n "Laster  "
    for i in {1..4}; do
        for s in "${symboler[@]}"; do
            echo -ne "\rLaster $s"
            sleep 0.125
        done
    done
    echo -e "\rModul klar!         "
}

loading_animasjon
